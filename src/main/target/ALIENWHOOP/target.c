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

/*




         \   |   _ _| __|  \ |\ \      /|  |  _ \  _ \ _ \
        _ \  |     |  _|  .  | \ \ \  / __ | (   |(   |__/
      _/  _\____|___|___|_|\_|  \_/\_/ _| _|\___/\___/_|


              Take me to your leader-board...



*/

#include <stdint.h>

#include "platform.h"
#include "drivers/io.h"

#include "drivers/timer.h"
#include "drivers/timer_def.h"
#include "drivers/dma.h"

/* Score: 10 m: 4, dmatt: 0 emc: 0 lmc: 0, ledmc 0, odmac: 0 adcc: 0 ledc: 0 tc: 0 tcc: 0 nt 1 mxt 1 */
/* T08 C4, T03 C3, T03 C2, T08 C1, T05 C1,  */
/* D2S7O0, D1S7O0, D1S5O0, D2S2O0, D1S2O0,  */
/* Usable motors:  M1 M2 M3 M4 */


// no change required for bidir

/* Currently only supporting brushed quad configuration e.g. Tiny Whoop. Care must be
 * taken to ensure functionality on both F4 and F7 (STM32F405RGT and STM32F722RET)
 */
const timerHardware_t timerHardware[USABLE_TIMER_CHANNEL_COUNT] = {
    DEF_TIM(TIM8, CH4, PC9, TIM_USE_MOTOR, 0, 0),
    DEF_TIM(TIM3, CH3, PC8, TIM_USE_MOTOR, 0, 0),
    DEF_TIM(TIM3, CH2, PC7, TIM_USE_MOTOR, 0, 0),
    DEF_TIM(TIM8, CH1, PC6, TIM_USE_MOTOR, 0, 0),

    DEF_TIM(TIM5, CH1, PA0, TIM_USE_LED,   0, 0),
};
