/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <math.h>
#include <string.h>

#include "platform.h"

#ifdef USE_BBSHOT

#include "build/debug.h"

#include "drivers/io.h"
#include "drivers/io_impl.h"
#include "drivers/dma.h"
#include "drivers/dma_reqmap.h"
#include "drivers/dshot.h"
#include "drivers/dshot_bb.h"
#include "drivers/dshot_bb_impl.h"
#include "drivers/dshot_command.h"
#include "drivers/motor.h"
#include "drivers/nvic.h"
#include "drivers/pwm_output.h" // XXX for pwmOutputPort_t motors[]; should go away with refactoring
#include "drivers/time.h"
#include "drivers/timer.h"

#include "pg/motor.h"

FAST_RAM_ZERO_INIT bbPacer_t bbPacers[MAX_MOTOR_PACERS];  // TIM1 or TIM8
FAST_RAM_ZERO_INIT int usedMotorPacers = 0;

FAST_RAM_ZERO_INIT bbPort_t bbPorts[MAX_SUPPORTED_MOTOR_PORTS];
FAST_RAM_ZERO_INIT int usedMotorPorts;

FAST_RAM_ZERO_INIT bbMotor_t bbMotors[MAX_SUPPORTED_MOTORS];

FAST_RAM_ZERO_INIT bbPort_t *gpioToMotorPort[10]; // GPIO group to bbPort mapping

// For MCUs that use MPU to control DMA coherency, there might be a performance hit
// on manipulating input buffer content especially if it is read multiple times,
// as the buffer region is attributed as not cachable.
// If this is not desirable, we should use manual cache invalidation.

#if defined(STM32F4)
#define BB_OUTPUT_BUFFER_ATTRIBUTE
#define BB_INPUT_BUFFER_ATTRIBUTE
#elif defined(STM32F7)
#define BB_OUTPUT_BUFFER_ATTRIBUTE FAST_RAM_ZERO_INIT
#define BB_INPUT_BUFFER_ATTRIBUTE  FAST_RAM_ZERO_INIT
#elif defined(STM32H7)
#define BB_OUTPUT_BUFFER_ATTRIBUTE DMA_RAM
#define BB_INPUT_BUFFER_ATTRIBUTE  DMA_RAM
#endif

BB_OUTPUT_BUFFER_ATTRIBUTE uint32_t bbOutputBuffer[MOTOR_PROSHOT1000_BUFFER_SIZE * MAX_SUPPORTED_MOTOR_PORTS];
BB_INPUT_BUFFER_ATTRIBUTE uint16_t bbInputBuffer[BBSHOT_PORT_INPUT_BUFFER_LENGTH * MAX_SUPPORTED_MOTOR_PORTS];

uint8_t bbPuPdMode;

// DMA GPIO output buffer formatting

void bbOutputDataInit(uint32_t *buffer, uint16_t portMask, bool inverted)
{
    uint32_t resetMask;
    uint32_t setMask;

    if (inverted) {
        resetMask = portMask;
        setMask = (portMask << 16);
    } else {
        resetMask = (portMask << 16);
        setMask = portMask;
    }

    int bitpos;

    for (bitpos = 0; bitpos < 16; bitpos++) {
        buffer[bitpos * 3 + 0] |= setMask ; // Always set all ports
        buffer[bitpos * 3 + 1] = 0;          // Reset bits are port dependent
        buffer[bitpos * 3 + 2] |= resetMask; // Always reset all ports
    }
}

void bbOutputDataSet(uint32_t *buffer, int portNumber, uint16_t value, bool inverted)
{
    uint32_t middleBit;

    if (inverted) {
        middleBit = (1 << (portNumber + 0));
    } else {
        middleBit = (1 << (portNumber + 16));
    }

    for (int pos = 0; pos < 16; pos++) {
        if (!(value & 0x8000)) {
            buffer[pos * 3 + 1] |= middleBit;
        }
        value <<= 1;
    }
}

void bbOutputDataClear(uint32_t *buffer)
{
    // Middle position to no change
    for (int bitpos = 0; bitpos < 16; bitpos++) {
        buffer[bitpos * 3 + 1] = 0;
    }
}

// bbPacer management

static bbPacer_t *bbFindMotorPacer(TIM_TypeDef *tim)
{
    for (int i = 0; i < MAX_MOTOR_PACERS; i++) {

        bbPacer_t *bbPacer = &bbPacers[i];

        if (bbPacer->tim == NULL) {
            bbPacer->tim = tim;
            ++usedMotorPacers;
            return bbPacer;
        }

        if (bbPacer->tim == tim) {
            return bbPacer;
        }
    }

    return NULL;
}

// bbPort management

static bbPort_t *bbFindMotorPort(int portIndex)
{
    return gpioToMotorPort[portIndex];
}

bbPort_t *bbAllocMotorPort(int portIndex)
{
    if (usedMotorPorts == MAX_SUPPORTED_MOTOR_PORTS) {
        return NULL;
    }

    bbPort_t *bbPort = &bbPorts[usedMotorPorts];

    if (!bbPort->timhw) {
        // No more pacer channel available
        return NULL;
    }

    gpioToMotorPort[portIndex] = bbPort;
    ++usedMotorPorts;

    return bbPort;
}

// Return frequency of smallest change [state/sec]

uint32_t getDshotBaseFrequency(motorPwmProtocolTypes_e pwmProtocolType)
{
    switch (pwmProtocolType) {
    case(PWM_TYPE_PROSHOT1000):
        return MOTOR_PROSHOT1000_SYMBOL_RATE * MOTOR_PROSHOT1000_STATE_PER_SYMBOL;
    case(PWM_TYPE_DSHOT1200):
        return MOTOR_DSHOT1200_SYMBOL_RATE * MOTOR_DSHOT_STATE_PER_SYMBOL;
    case(PWM_TYPE_DSHOT600):
        return MOTOR_DSHOT600_SYMBOL_RATE * MOTOR_DSHOT_STATE_PER_SYMBOL;
    case(PWM_TYPE_DSHOT300):
        return MOTOR_DSHOT300_SYMBOL_RATE * MOTOR_DSHOT_STATE_PER_SYMBOL;
    default:
    case(PWM_TYPE_DSHOT150):
        return MOTOR_DSHOT150_SYMBOL_RATE * MOTOR_DSHOT_STATE_PER_SYMBOL;
    }
}

static void bbAllocDMA(bbPort_t *bbPort)
{
    const timerHardware_t *timhw = bbPort->timhw;

#ifdef USE_DMA_SPEC
    const dmaChannelSpec_t *dmaChannelSpec = dmaGetChannelSpecByTimer(timhw);
    bbPort->dmaResource = dmaChannelSpec->ref;
    bbPort->dmaChannel = dmaChannelSpec->channel;
#else
    bbPort->dmaResource = timhw->dmaRef;
    bbPort->dmaChannel = timhw->dmaChannel;
#endif

    dmaIdentifier_e dmaIdentifier = dmaGetIdentifier(bbPort->dmaResource);
    dmaInit(dmaIdentifier, OWNER_BBSHOT, RESOURCE_INDEX(bbPort - bbPorts));
    bbPort->dmaSource = timerDmaSource(timhw->channel);

    bbPacer_t *bbPacer = bbFindMotorPacer(timhw->tim);
    bbPacer->dmaSources |= bbPort->dmaSource;

    dmaSetHandler(dmaIdentifier, bbDMAIrqHandler, NVIC_BUILD_PRIORITY(2, 1), (uint32_t)bbPort);

    bbDMA_ITConfig(bbPort);
}

void bbDMAIrqHandler(dmaChannelDescriptor_t *descriptor)
{
    dbgPinHi(0);

    bbPort_t *bbPort = (bbPort_t *)descriptor->userParam;

    bbDMA_Cmd(bbPort, DISABLE);

    bbTIM_DMACmd(bbPort->timhw->tim, bbPort->dmaSource, DISABLE);

    if (DMA_GET_FLAG_STATUS(descriptor, DMA_IT_TEIF)) {
        while (1) {};
    }

    DMA_CLEAR_FLAG(descriptor, DMA_IT_TCIF);

#ifdef USE_DSHOT_TELEMETRY
    if (useDshotTelemetry) {
        if (bbPort->direction == BBSHOT_DIRECTION_INPUT) {
#ifdef DEBUG_COUNT_INTERRUPT
            bbPort->inputIrq++;
#endif

            // Decode input

            // Switch to output

            bbPort->inputActive = false;
            bbSwitchToOutput(bbPort);
        } else {
#ifdef DEBUG_COUNT_INTERRUPT
            bbPort->outputIrq++;
#endif

            // Switch to input

            bbSwitchToInput(bbPort);

            bbTIM_DMACmd(bbPort->timhw->tim, bbPort->dmaSource, ENABLE);
        }
    }
#endif
    dbgPinLo(0);
}

// Setup bbPorts array elements so that they each have a TIM1 or TIM8 channel
// in timerHardware array for BB-DShot.

static void bbFindPacerTimer(void)
{
    const timerHardware_t *timhw;

    for (int bbPortIndex = 0; bbPortIndex < MAX_SUPPORTED_MOTOR_PORTS; bbPortIndex++) {
        timhw = timerGetByUsage(TIM_USE_BBSHOT, bbPortIndex);

        if (!timhw) {
            break;
        }

        bbPorts[bbPortIndex].timhw = timhw;
    }
}

void bbTimebaseSetup(bbPort_t *bbPort, motorPwmProtocolTypes_e dshotProtocolType)
{
    uint32_t timerclock = timerClock(bbPort->timhw->tim);

    uint32_t outputFreq = getDshotBaseFrequency(dshotProtocolType);
    bbPort->outputARR = timerclock / outputFreq - 1;

    // XXX Explain this formula
    uint32_t inputFreq = outputFreq * 5 * 2 * BBSHOT_TELEMETRY_OVER_SAMPLE / 24;
    bbPort->inputARR = timerclock / inputFreq - 1;
}

//
// bb only use pin info associated with timerHardware entry designated as TIM_USE_MOTOR;
// it does not use the timer channel associated with the pin.
//

bool bbMotorConfig(const timerHardware_t *timhw, uint8_t motorIndex, motorPwmProtocolTypes_e pwmProtocolType, uint8_t output)
{
    UNUSED(output); // Not supported

    IO_t io = IOGetByTag(timhw->tag);

    int pinIndex = IO_GPIOPinIdx(io);
    int portIndex = IO_GPIOPortIdx(io);

    bbPort_t *bbPort = bbFindMotorPort(portIndex);

    if (!bbPort) {

        // New port group

        bbPort = bbAllocMotorPort(portIndex);
        if (!bbPort) {
            return false;
        }

        bbPort->gpio = IO_GPIO(io);

        uint16_t bufferSize = (pwmProtocolType == PWM_TYPE_PROSHOT1000) ? MOTOR_PROSHOT1000_BUFFER_SIZE : MOTOR_DSHOT_BUFFER_SIZE;

        bbPort->portOutputCount = bufferSize;
        bbPort->portOutputBuffer = &bbOutputBuffer[(bbPort - bbPorts) * bufferSize];

        bbPort->portInputCount = BBSHOT_PORT_INPUT_BUFFER_LENGTH;
        bbPort->portInputBuffer = &bbInputBuffer[(bbPort - bbPorts) * BBSHOT_PORT_INPUT_BUFFER_LENGTH];

        bbTimebaseSetup(bbPort, pwmProtocolType);
        bbTIM_TimeBaseInit(bbPort, bbPort->outputARR);
        bbTimerChannelInit(bbPort);

        bbAllocDMA(bbPort);
        bbDMAPreconfigure(bbPort, BBSHOT_DIRECTION_OUTPUT);
        bbDMAPreconfigure(bbPort, BBSHOT_DIRECTION_INPUT);

        bbDMA_ITConfig(bbPort);
    }

    bbMotors[motorIndex].pinIndex = pinIndex;
    bbMotors[motorIndex].io = io;
    bbMotors[motorIndex].bbPort = bbPort;

    IOInit(io, OWNER_MOTOR, RESOURCE_INDEX(motorIndex));

    // Setup GPIO_MODER and GPIO_ODR register manipulation values

    bbGpioSetup(&bbMotors[motorIndex]);

#ifdef USE_DSHOT_TELEMETRY
    if (useDshotTelemetry) {
        bbOutputDataInit(bbPort->portOutputBuffer, (1 << pinIndex), BBSHOT_INVERTED);
    } else
#endif
    {
        bbOutputDataInit(bbPort->portOutputBuffer, (1 << pinIndex), BBSHOT_NONINVERTED);
    }

    bbSwitchToOutput(bbPort);

    bbMotors[motorIndex].configured = true;

    return true;
}

static FAST_RAM_ZERO_INIT motorDevice_t bbDevice;

static bool bbUpdateStart(void)
{
    for (int i = 0; i < usedMotorPorts; i++) {
        bbOutputDataClear(bbPorts[i].portOutputBuffer);
    }

    return true;
}

static void bbWriteInt(uint8_t motorIndex, uint16_t value)
{
    bbMotor_t *const bbmotor = &bbMotors[motorIndex];

    if (!bbmotor->configured) {
        return;
    }

    // If there is a command ready to go overwrite the value and send that instead

    if (dshotCommandIsProcessing()) {
        value = dshotCommandGetCurrent(motorIndex);
        if (value) {
            bbmotor->protocolControl.requestTelemetry = true;
        }
    }

    bbmotor->protocolControl.value = value;

    uint16_t packet = prepareDshotPacket(&bbmotor->protocolControl);

    bbPort_t *bbPort = bbmotor->bbPort;

#ifdef USE_DSHOT_TELEMETRY
    if (useDshotTelemetry) {
        bbOutputDataSet(bbPort->portOutputBuffer, bbmotor->pinIndex, packet, BBSHOT_INVERTED);
    } else
#endif
    {
        bbOutputDataSet(bbPort->portOutputBuffer, bbmotor->pinIndex, packet, BBSHOT_NONINVERTED);
    }
}

static void bbWrite(uint8_t motorIndex, float value)
{
    bbWriteInt(motorIndex, value);
}

static void bbUpdateComplete(void)
{
    // If there is a dshot command loaded up, time it correctly with motor update

    if (!dshotCommandQueueEmpty()) {
        if (!dshotCommandOutputIsEnabled(bbDevice.count)) {
            return;
        }
    }

    for (int i = 0; i < usedMotorPorts; i++) {
        bbPort_t *bbPort = &bbPorts[i];

        // XXX Just for testing output side without input side DMA working
        bbSwitchToOutput(bbPort);

        bbDMA_Cmd(bbPort, ENABLE);
    }

    for (int i = 0; i < usedMotorPacers; i++) {
        bbPacer_t *bbPacer = &bbPacers[i];
        bbTIM_DMACmd(bbPacer->tim, bbPacer->dmaSources, ENABLE);
    }
}

static bool bbEnableMotors(void)
{
    return true;
}

static void bbDisableMotors(void)
{
    return;
}

static void bbShutdown(void)
{
    return;
}

static bool bbIsMotorEnabled(uint8_t index)
{
    return bbMotors[index].enabled;
}

static motorVTable_t bbVTable = {
    .enable = bbEnableMotors,
    .disable = bbDisableMotors,
    .isMotorEnabled = bbIsMotorEnabled,
    .updateStart = bbUpdateStart,
    .write = bbWrite,
    .writeInt = bbWriteInt,
    .updateComplete = bbUpdateComplete,
    .convertExternalToMotor = dshotConvertFromExternal,
    .convertMotorToExternal = dshotConvertToExternal,
    .shutdown = bbShutdown,
};

motorDevice_t *bbshotDevInit(const motorDevConfig_t *motorConfig, uint16_t idlePulse, uint8_t motorCount, bool useUnsyncedPwm)
{
    UNUSED(idlePulse);
    UNUSED(useUnsyncedPwm);

    dbgPinInit();
    dbgPinLo(0);
    dbgPinLo(1);

    bbFindPacerTimer();

    bbDevice.vTable = bbVTable;

#ifdef USE_DSHOT_TELEMETRY
    useDshotTelemetry = motorConfig->useDshotTelemetry;
#endif

#ifdef USE_DSHOT_TELEMETRY
    if (useDshotTelemetry) {
        bbPuPdMode = BB_GPIO_PULLUP;
    } else
#endif
    {
        bbPuPdMode = BB_GPIO_PULLDOWN;
    }

    for (int motorIndex = 0; motorIndex < MAX_SUPPORTED_MOTORS && motorIndex < motorCount; motorIndex++) {
        const ioTag_t tag = motorConfig->ioTags[motorIndex];
        const timerHardware_t *timerHardware = timerAllocate(tag, OWNER_MOTOR, RESOURCE_INDEX(motorIndex));

        if (timerHardware == NULL) {
            /* not enough motors initialised for the mixer or a break in the motors */
            bbDevice.vTable.write = motorWriteNull;
            bbDevice.vTable.updateComplete = motorUpdateCompleteNull;
            /* TODO: block arming and add reason system cannot arm */
            return NULL;
        }

        // motors[motorIndex].io = IOGetByTag(tag);
        IOInit(bbMotors[motorIndex].io, OWNER_MOTOR, RESOURCE_INDEX(motorIndex));

        if (!bbMotorConfig(timerHardware, motorIndex, motorConfig->motorPwmProtocol, motorConfig->motorPwmInversion ? timerHardware->output ^ TIMER_OUTPUT_INVERTED : timerHardware->output)) {
            return NULL;
        }

        bbMotors[motorIndex].enabled = true;

        // Fill in motors structure for 4way access (XXX Should be refactored)

        motors[motorIndex].io = bbMotors[motorIndex].io;
        motors[motorIndex].enabled = true;
    }

    return &bbDevice;
}

typedef struct dbgPin_s {
    ioTag_t tag;
    IO_t io;
    GPIO_TypeDef *gpio;
    uint32_t setBSRR;
    uint32_t resetBSRR;
} dbgPin_t;

dbgPin_t dbgPins[] = {
#ifdef OMNIBUSF4SD
    { .tag = IO_TAG(PA1) },
    { .tag = IO_TAG(PA8) },
#endif
};

void dbgPinInit(void)
{
    for (unsigned i = 0; i < ARRAYLEN(dbgPins); i++) {
        dbgPin_t *dbgPin = &dbgPins[i];
        dbgPin->io = IOGetByTag(dbgPin->tag);
        if (!dbgPin->io) {
            continue;
        }
        IOConfigGPIO(dbgPin->io, IOCFG_OUT_PP);
        dbgPin->gpio = IO_GPIO(dbgPin->io);
        int pinSrc = IO_GPIO_PinSource(dbgPin->io);
        dbgPin->setBSRR = (1 << pinSrc);
        dbgPin->resetBSRR = (1 << (pinSrc + 16));
    }
}

void dbgPinHi(int index)
{
    if ((unsigned)index > ARRAYLEN(dbgPins)) {
        return;
    }

    dbgPin_t *dbgPin = &dbgPins[index];

    dbgPin->gpio->BSRRL = dbgPin->setBSRR;
}

void dbgPinLo(int index)
{
    if ((unsigned)index > ARRAYLEN(dbgPins)) {
        return;
    }

    dbgPin_t *dbgPin = &dbgPins[index];

    dbgPin->gpio->BSRRL = dbgPin->resetBSRR;
}

#endif // USE_DSHOT_BB
