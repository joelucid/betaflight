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
/*
Score: 124 m: 8, dmatt: 0 emc: 0 lmc: 2, ledmc 0, odmac: 2 adcc: 0 ledc: 3 tc: 0 tcc: 0 nt 2 mxt 4
T12 C1, T12 C2, T08 C1, T08 C2, T08 C3, T08 C4, T04 C3, T05 C3, T04 C2, T05 C4, T02 C2, T03 C3, T03 C2, T02 C1, T03 C4, T03 C1, ADC1 
D0S0O0, D0S0O0, D2S2O0, D2S3O1, D2S4O1, D2S7O0, D1S7O0, D1S0O0, D1S3O0, D1S1O0, D1S6O0, D1S7O0, D1S5O0, D1S5O0, D1S2O0, D1S4O0, D2 S0 
Usable motors:  M1 M2 M3 M4 M5 M7 M9 M10

*/

// bidir changed

const timerHardware_t timerHardware[USABLE_TIMER_CHANNEL_COUNT] = {
    DEF_TIM(TIM12, CH1,  PB14, TIM_USE_PWM | TIM_USE_PPM,   0, 0 ), // S1_IN
    DEF_TIM(TIM12, CH2,  PB15, TIM_USE_PWM,                 0, 0 ), // S2_IN
    DEF_TIM(TIM8,  CH1,  PC6,  TIM_USE_PWM,                 0, 0 ), // S3_IN DMA2_ST2 DMA2_ST2
    DEF_TIM(TIM8,  CH2,  PC7,  TIM_USE_PWM,                 0, 1 ), // S4_IN DMA2_ST3 DMA2_ST2
    DEF_TIM(TIM8,  CH3,  PC8,  TIM_USE_PWM,                 0, 1 ), // S5_IN DMA2_ST4 DMA2_ST2
    DEF_TIM(TIM8,  CH4,  PC9,  TIM_USE_PWM,                 0, 0 ), // S6_IN DMA2_ST7

    DEF_TIM(TIM4,  CH3,  PB8,  TIM_USE_MOTOR,               0, 0 ), // S10_OUT 1 DMA1_ST7
    DEF_TIM(TIM5,  CH3,  PA2,  TIM_USE_MOTOR,               0, 0 ), // S6_OUT  2 DMA1_ST1
    DEF_TIM(TIM4,  CH2,  PB7,  TIM_USE_MOTOR,               0, 0 ), // S5_OUT  3 DMA1_ST3
    DEF_TIM(TIM5,  CH4,  PA3,  TIM_USE_MOTOR,               0, 0 ), // S1_OUT  4 DMA1_ST7 DMA1_ST6
    DEF_TIM(TIM2,  CH2,  PA1,  TIM_USE_MOTOR,               0, 0 ), // S2_OUT DMA1_ST4
    DEF_TIM(TIM3,  CH3,  PB0,  TIM_USE_MOTOR,               0, 0 ), // S3_OUT DMA2_ST6 DMA2_ST2
    DEF_TIM(TIM3,  CH2,  PB5,  TIM_USE_MOTOR | TIM_USE_LED, 0, 0 ), // S4_OUT DMA1_ST5
    DEF_TIM(TIM2,  CH1,  PA0,  TIM_USE_MOTOR,               0, 0 ), // S7_OUT DMA1_ST2
    DEF_TIM(TIM3,  CH4 , PB1,  TIM_USE_MOTOR,               0, 0 ), // S8_OUT DMA2_ST6
    DEF_TIM(TIM3,  CH1,  PB4,  TIM_USE_MOTOR,               0, 0 ), // S9_OUT DMA1_ST4
};
