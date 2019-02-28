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

/* Score: 110 m: 4, dmatt: 0 emc: 1 lmc: 0, ledmc 0, odmac: 0 adcc: 0 ledc: 0 tc: 0 tcc: 0 nt 1 mxt 1 */
/* T02 C2, T04 C1, T04 C2, T04 C3, T04 C4, T03 C4, ADC1  */
/* D1S6O0, D1S0O0, D1S3O0, D1S7O0, D0S0O0, D1S2O0, D2 S0  */
/* Usable motors:  M1 M2 M3 M5 */

// bidir no change. use led pin instead of PB9

const timerHardware_t timerHardware[USABLE_TIMER_CHANNEL_COUNT] = {

    DEF_TIM(TIM2,  CH2,   PB3, TIM_USE_CAMERA_CONTROL,     0, 0), // USE FOR CAMERA CONTROL

    DEF_TIM(TIM4,  CH1,  PB6, TIM_USE_MOTOR,               0, 0), // D1-ST6
    DEF_TIM(TIM4,  CH2,  PB7, TIM_USE_MOTOR,               0, 0), // D1-ST6
    DEF_TIM(TIM4,  CH3,  PB8, TIM_USE_MOTOR,               0, 0), // D1-ST6
    DEF_TIM(TIM4,  CH4,  PB9, TIM_USE_MOTOR,               0, 0), // D1-ST6

    DEF_TIM(TIM3,  CH4, PB1, TIM_USE_MOTOR | TIM_USE_LED,  0, 0), // D1-ST2


};
