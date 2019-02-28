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

/* Score: 309 m: 6, dmatt: 0 emc: 0 lmc: 0, ledmc 0, odmac: 0 adcc: 0 ledc: 0 tc: 30 tcc: 0 nt 2 mxt 1 */
/* T01 C3, T03 C3, T03 C4, T01 C1, T01 C2, T08 C4, T05 C4, T04 C1, ADC1  */
/* D2S6O1, D1S7O0, D1S2O0, D2S3O2, D2S2O1, D2S7O0, D1S1O0, D1S0O0, D2 S0  */
/* Usable motors:  M1 M2 M3 M4 M5 M6 */

// bidir no change

const timerHardware_t timerHardware[USABLE_TIMER_CHANNEL_COUNT] = {
    DEF_TIM(TIM1, CH3, PE13, TIM_USE_PPM,   0, 1), // PPM, DMA2_ST6

    DEF_TIM(TIM3, CH3, PB0,  TIM_USE_MOTOR, 0, 0), // M1 , DMA1_ST7
    DEF_TIM(TIM3, CH4, PB1,  TIM_USE_MOTOR, 0, 0), // M2 , DMA1_ST2
    DEF_TIM(TIM1, CH1, PE9,  TIM_USE_MOTOR, 0, 2), // M3 , DMA2_ST2
    DEF_TIM(TIM1, CH2, PE11, TIM_USE_MOTOR, 0, 1), // M4 , DMA2_ST4
    DEF_TIM(TIM8, CH4, PC9,  TIM_USE_MOTOR, 0, 0), // M5 , DMA2_ST7
    DEF_TIM(TIM5, CH4, PA3,  TIM_USE_MOTOR, 0, 0), // M6 , DMA1_ST1

    DEF_TIM(TIM4, CH1, PD12, TIM_USE_LED,   0, 0), // LED_TRIP, DMA1_ST0
};

