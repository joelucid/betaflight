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

#include "platform.h"
#include "drivers/io.h"

#include "drivers/dma.h"
#include "drivers/timer.h"
#include "drivers/timer_def.h"

// DSHOT will work for motor 1-8.
// If SDCard or UART4 DMA is used motor 7 will not work.
// If UART1 DMA is used motor 8 will not work.

/* Score: 140 m: 8, dmatt: 0 emc: 1 lmc: 3, ledmc 0, odmac: 0 adcc: 0 ledc: 0 tc: 0 tcc: 0 nt 1 mxt 1 */
/* T01 C1, T08 C1, T08 C2, T12 C1, T03 C3, T02 C1, T08 C3, T02 C2, T08 C4, T03 C4, T12 C2, T10 C1, T11 C1, ADC1  */
/* D2S6O0, D2S2O1, D2S3O1, D0S0O0, D1S7O0, D1S5O0, D2S4O1, D1S6O0, D2S7O0, D1S2O0, D0S0O0, D0S0O0, D0S0O0, D2 S0  */
/* Usable motors:  M1 M2 M4 M5 M6 M7 M8 M9 */


const timerHardware_t timerHardware[USABLE_TIMER_CHANNEL_COUNT] = {
    DEF_TIM(TIM1, CH1,  PA8,  TIM_USE_PPM | TIM_USE_LED, 0, 0), // PPM   - DMA2_ST6, *DMA2_ST1, DMA2_ST3
    DEF_TIM(TIM8, CH1,  PC6,  TIM_USE_MOTOR,             0, 1), // PWM1  - DMA2_ST2, DMA2_ST2
    DEF_TIM(TIM8, CH2,  PC7,  TIM_USE_MOTOR,             0, 1), // PWM2  - DMA1_ST5
    DEF_TIM(TIM12, CH1, PB14, TIM_USE_MOTOR,             0, 0), // PWM3  - DMA2_ST3, DMA2_ST2
    DEF_TIM(TIM3, CH3,  PB0,  TIM_USE_MOTOR,             0, 0), // PWM4  - DMA1_ST7
    DEF_TIM(TIM2, CH1,  PA0,  TIM_USE_MOTOR,             0, 0), // PWM5  - DMA1_ST2
    DEF_TIM(TIM8, CH3,  PC8,  TIM_USE_MOTOR,             0, 1), // PWM6  - (DMA2_ST4) DMA2_ST2
    DEF_TIM(TIM2, CH2,  PA1,  TIM_USE_MOTOR,             0, 0), // PWM7  - (DMA1_ST4)             - DMA SDCard, DMA Serial_TX4
    DEF_TIM(TIM8, CH4,  PC9,  TIM_USE_MOTOR,             0, 0), // PWM8  - (DMA2_ST7)             - DMA Serial_TX1
    DEF_TIM(TIM3, CH4,  PB1,  TIM_USE_MOTOR,             0, 0), // PWM9  - (DMA1_ST2)             - Collision
    DEF_TIM(TIM12, CH2, PB15, TIM_USE_MOTOR,             0, 0), // PWM10 - (DMA2_ST6), (DMA2_ST6) - Collision
    DEF_TIM(TIM10, CH1,  PB8,  TIM_USE_MOTOR,             0, 0), // PWM11 - (DMA1_ST7)             - Collision
    DEF_TIM(TIM11, CH1,  PB9,  TIM_USE_MOTOR,             0, 0), // PWM12 - DMA_NONE
};
