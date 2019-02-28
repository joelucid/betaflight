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

/* Score: 121 m: 4, dmatt: 0 emc: 1 lmc: 1, ledmc 0, odmac: 1 adcc: 0 ledc: 0 tc: 0 tcc: 0 nt 2 mxt 2 */
/* T12 C2, T08 C1, T08 C2, T04 C3, T02 C4, T03 C2, T04 C4, T09 C2, T03 C1, ADC1  */
/* D0S0O0, D2S2O0, D2S3O1, D1S7O0, D1S6O1, D1S5O0, D0S0O0, D0S0O0, D1S4O0, D2 S0  */
/* Usable motors:  M1 M2 M3 M6 */

// bidir no change. PB9 and PE6 do not have dma.

const timerHardware_t timerHardware[USABLE_TIMER_CHANNEL_COUNT] = {

    DEF_TIM(TIM12, CH2, PB15, TIM_USE_PWM | TIM_USE_PPM, 0, 0),
    DEF_TIM(TIM8,  CH1, PC6,  TIM_USE_PWM,               0, 0),
    DEF_TIM(TIM8,  CH2, PC7,  TIM_USE_PWM,               0, 1),
//    DEF_TIM(TIM8,  CH4, PC9,  TIM_USE_PWM,               0, 0), // Used for SDIO
//    DEF_TIM(TIM8,  CH3, PC8,  TIM_USE_PWM,               0, 1), // Used for SDIO

    DEF_TIM(TIM4,  CH3, PB8,  TIM_USE_MOTOR,             0, 0),
    DEF_TIM(TIM2,  CH4, PA3,  TIM_USE_MOTOR,             0, 1),
    DEF_TIM(TIM3,  CH2, PB5,  TIM_USE_MOTOR,             0, 0),
    DEF_TIM(TIM4,  CH4, PB9,  TIM_USE_MOTOR,             0, 0),
    DEF_TIM(TIM9,  CH2, PE6,  TIM_USE_MOTOR,             0, 0),
    DEF_TIM(TIM3,  CH1, PB4,  TIM_USE_MOTOR,             0, 0),
};
