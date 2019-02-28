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

#include "drivers/timer.h"
#include "drivers/timer_def.h"
#include "drivers/dma.h"

/* Score: 9 m: 8, dmatt: 0 emc: 0 lmc: 0, ledmc 0, odmac: 1 adcc: 0 ledc: 0 tc: 0 tcc: 0 nt 3 mxt 1 */
/* T02 C1, T09 C2, T08 C3, T08 C1, T08 C4, T08 C2, T04 C1, T04 C2, T03 C4, T03 C3, T05 C2, ADC3  */
/* D1S5O0, D0S0O0, D2S4O1, D2S2O1, D2S7O0, D2S3O1, D1S0O0, D1S3O0, D1S2O0, D1S7O0, D1S4O0, D2 S0  */
/* Usable motors:  M1 M2 M3 M4 M5 M6 M7 M8 */

const timerHardware_t timerHardware[USABLE_TIMER_CHANNEL_COUNT] = {
    DEF_TIM(TIM2,  CH1, PA0,  TIM_USE_CAMERA_CONTROL, 0, 0),

    DEF_TIM(TIM9,  CH2, PA3,  TIM_USE_PPM ,   0, 0), // PPM

    DEF_TIM(TIM8,  CH3, PC8,  TIM_USE_MOTOR,  0, 1), // ESC 1
    DEF_TIM(TIM8,  CH1, PC6,  TIM_USE_MOTOR,  0, 1), // ESC 2
    DEF_TIM(TIM8,  CH4, PC9,  TIM_USE_MOTOR,  0, 0), // ESC 3
    DEF_TIM(TIM8,  CH2, PC7,  TIM_USE_MOTOR,  0, 1), // ESC 4
    DEF_TIM(TIM4,  CH1, PB6,  TIM_USE_MOTOR,  0, 0), // ESC 5 
    DEF_TIM(TIM4,  CH2, PB7,  TIM_USE_MOTOR,  0, 0), // ESC 6
    DEF_TIM(TIM3,  CH4, PB1,  TIM_USE_MOTOR,  0, 0), // ESC 7
    DEF_TIM(TIM3,  CH3, PB0,  TIM_USE_MOTOR,  0, 0), // ESC 8

    DEF_TIM(TIM5,  CH2, PA1,  TIM_USE_LED,    0, 0), // LED Strip
};
