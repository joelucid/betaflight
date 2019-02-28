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

/* Score: 39 m: 6, dmatt: 0 emc: 0 lmc: 0, ledmc 0, odmac: 1 adcc: 0 ledc: 1 tc: 0 tcc: 0 nt 3 mxt 1 */
/* T08 C3, T02 C1, T05 C2, T05 C3, T05 C4, T03 C3, T03 C4, T04 C2, T12 C1, ADC1  */
/* D2S2O0, D1S5O0, D1S4O0, D1S0O0, D1S1O0, D1S7O0, D1S2O0, D1S3O0, D0S0O0, D2 S0  */
/* Usable motors:  M1 M2 M3 M4 M5 M6 */

// bidir no change

const timerHardware_t timerHardware[USABLE_TIMER_CHANNEL_COUNT] = {
    DEF_TIM(TIM8,  CH3, PC8,  TIM_USE_PPM,                 0, 1 ), // PPM IN
    DEF_TIM(TIM2,  CH1, PA0,  TIM_USE_MOTOR,               0, 0 ), // S1_OUT - DMA1_ST2
    DEF_TIM(TIM5,  CH2, PA1,  TIM_USE_MOTOR,               0, 0 ), // S2_OUT - DMA1_ST4
    DEF_TIM(TIM5,  CH3, PA2,  TIM_USE_MOTOR,               0, 0 ), // S3_OUT - DMA1_ST1
    DEF_TIM(TIM5,  CH4, PA3,  TIM_USE_MOTOR,               0, 1 ), // S4_OUT - DMA1_ST6
    DEF_TIM(TIM3,  CH3, PB0,  TIM_USE_MOTOR,               0, 0 ), // S5_OUT - DMA1_ST7 - LED Control
    DEF_TIM(TIM3,  CH4, PB1,  TIM_USE_MOTOR | TIM_USE_LED, 0, 0 ), // S6_OUT
    DEF_TIM(TIM4,  CH2, PB7,  TIM_USE_CAMERA_CONTROL,      0, 0 ), // Camera Control
    DEF_TIM(TIM12, CH1, PB14, TIM_USE_BEEPER,              0, 0 ), // BEEPER PWM
};

