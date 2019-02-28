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

/* Score: 23 m: 7, dmatt: 0 emc: 0 lmc: 1, ledmc 0, odmac: 3 adcc: 0 ledc: 0 tc: 0 tcc: 0 nt 3 mxt 3 */
/* T03 C1, T03 C2, T03 C3, T03 C4, T02 C1, T02 C2, T04 C1, T04 C2, T01 C1, T05 C4, T09 C1, T05 C2, T05 C1, ADC1  */
/* D1S4O0, D1S5O0, D1S7O0, D1S2O0, D1S5O0, D1S6O0, D1S0O0, D1S3O0, D2S3O2, D1S1O0, D0S0O0, D1S4O0, D1S2O0, D2 S0  */
/* Usable motors:  M1 M2 M3 M4 M6 M7 M8 */

// bidir only conflict S5 vs S2

const timerHardware_t timerHardware[USABLE_TIMER_CHANNEL_COUNT] = {

    DEF_TIM(TIM3, CH1, PB4,   TIM_USE_MOTOR, 0, 0),   // S1   UP1-2
    DEF_TIM(TIM3, CH2, PB5,   TIM_USE_MOTOR, 0, 0),   // S2   UP1-2
    DEF_TIM(TIM3, CH3, PB0,   TIM_USE_MOTOR, 0, 0),   // S3   UP1-2
    DEF_TIM(TIM3, CH4, PB1,   TIM_USE_MOTOR, 0, 0),   // S4   UP1-2

    DEF_TIM(TIM2, CH1, PA15,  TIM_USE_MOTOR, 0, 0),   // S5   UP1-7
    DEF_TIM(TIM2, CH2, PB3,   TIM_USE_MOTOR, 0, 0),   // S6   UP1-7
    DEF_TIM(TIM4, CH1, PB6,   TIM_USE_MOTOR, 0, 0),   // S7   UP1-6
    DEF_TIM(TIM4, CH2, PB7,   TIM_USE_MOTOR, 0, 0),   // S8   UP1-6

    DEF_TIM(TIM1, CH1, PA8,  TIM_USE_LED, 0, 2),   // LED DMA2-3

    DEF_TIM(TIM5, CH4, PA3,  TIM_USE_PPM, 0, 0),   // PPM, RX2
    DEF_TIM(TIM9, CH1, PA2,  TIM_USE_PWM, 0, 0),   // TX2
    DEF_TIM(TIM5, CH2, PA1,  TIM_USE_PWM, 0, 0),   // RX4  
    DEF_TIM(TIM5, CH1, PA0,  TIM_USE_PWM, 0, 0),   // TX4

};
