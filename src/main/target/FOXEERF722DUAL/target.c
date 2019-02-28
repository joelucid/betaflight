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

/* Score: 20 m: 6, dmatt: 0 emc: 0 lmc: 0, ledmc 0, odmac: 0 adcc: 0 ledc: 0 tc: 1 tcc: 0 nt 2 mxt 2 */
/* T04 C2, T01 C2, T01 C1, T03 C4, T03 C3, T03 C1, T08 C2, T02 C1, T02 C2, ADC1  */
/* D1S3O0, D2S2O1, D2S1O1, D1S2O0, D1S7O0, D1S4O0, D2S3O1, D1S5O0, D1S6O0, D2 S0  */
/* Usable motors:  M1 M2 M3 M4 M5 M6 */


// 6 motors and led work with bidir


const timerHardware_t timerHardware[USABLE_TIMER_CHANNEL_COUNT] = {
    DEF_TIM(TIM4, CH2, PB7,  TIM_USE_PPM,   0, 0), // PPM&SBUS  

    DEF_TIM(TIM1, CH2, PA9,  TIM_USE_MOTOR, 0, 1),   // S1
    DEF_TIM(TIM1, CH1, PA8,  TIM_USE_MOTOR, 0, 1),   // S2
    DEF_TIM(TIM3, CH4, PC9,  TIM_USE_MOTOR, 0, 0),    // S3
    DEF_TIM(TIM3, CH3, PC8,  TIM_USE_MOTOR, 0, 0),   // S4
    DEF_TIM(TIM3, CH1, PC6,  TIM_USE_MOTOR, 0, 0),   // S5
    DEF_TIM(TIM8, CH2, PC7,  TIM_USE_MOTOR, 0, 1),   // S6


    DEF_TIM(TIM2, CH1, PA15,  TIM_USE_LED, 0, 0),    // LED STRIP(1,5)

    DEF_TIM(TIM2, CH2, PB3,  TIM_USE_CAMERA_CONTROL, 0, 0),     // FC CAM	
	
};
