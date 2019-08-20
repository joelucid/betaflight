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

#include <math.h>
#include "platform.h"

#ifdef USE_INTERPOLATED_SP

#include "build/debug.h"
#include "common/maths.h"
#include "fc/rc.h"
#include "flight/interpolated_setpoint.h"

static float avgSetpointSpeed[XYZ_AXIS_COUNT];
static float prevSetpointSpeed[XYZ_AXIS_COUNT];
static float prevRawSetpoint[XYZ_AXIS_COUNT];
static float boostAmount[XYZ_AXIS_COUNT];
static float prevBoostAmount[XYZ_AXIS_COUNT];

// Configuration
static float ffSpread;
static float ffMaxRateLimit[XYZ_AXIS_COUNT];
static float ffMaxRate[XYZ_AXIS_COUNT];

void interpolatedSpInit(const pidProfile_t *pidProfile) {
    const float ffMaxRateScale = pidProfile->ff_max_rate_limit * 0.01f;
    ffSpread = pidProfile->ff_spread;
    for (int i = 0; i < XYZ_AXIS_COUNT; i++) {
        ffMaxRate[i] = applyCurve(i, 1.0f);
        ffMaxRateLimit[i] = ffMaxRate[i] * ffMaxRateScale;
    }
}

FAST_CODE_NOINLINE float interpolatedSpApply(int axis, float pidFrequency, bool newRcFrame) {

    if (newRcFrame) {
        float rawSetpoint = getRawSetpoint(axis); 
        
        const float interpolationSteps = currentRxRefreshRate * pidFrequency * 1e-6f;
        const float setpointChange = rawSetpoint - prevRawSetpoint[axis];
        const float setpointSpeed = setpointSpeed / interpolationSteps;

        
        if (ffSpread != 0.0f && currentRxRefreshRate < 15000.0f) {
            avgSetpointSpeed[axis] = (setpointSpeed + prevSetpointSpeed[axis]) * 0.5f;
        } else {
            avgSetpointSpeed[axis] = setpointSpeed;
        }
        
        const float ffBoostFactor = pidGetFfBoostFactor();
        if (ffBoostFactor != 0.0f) {
            float boost = 0.0f;
            // prevent kick-back spike at max deflection
            if (fabsf(rawSetpoint) < 0.95f * ffMaxRate[axis] || fabsf(setpointSpeed) > 3.0f * fabsf(prevSetpointSpeed[axis])) {
                const float setpointAcc = (avgSetpointSpeed[axis] - prevSetpointSpeed[axis]) * interpolationSteps;
                boost = ffBoostFactor * setpointAcc / (currentRxRefreshRate * pidFrequency * 1e-6f);
                if (ffSpread) {
                    boost = (boost + prevBoostAmount[axis]) / 2;
                }
                boostAmount[axis] = (boost + prevBoostAmount[axis]) / 2;
            } else {
                boostAmount[axis] = 0;
            }
            prevBoostAmount[axis] = boost;
        }
        prevSetpointSpeed[axis] = setpointSpeed;
        prevRawSetpoint[axis] = rawSetpoint;
        
        if (axis == FD_ROLL) {
            DEBUG_SET(DEBUG_FF_INTERPOLATED, 0, avgSetpointSpeed[axis] * 10);
            DEBUG_SET(DEBUG_FF_INTERPOLATED, 1, boostAmount[axis] * 10);
            DEBUG_SET(DEBUG_FF_INTERPOLATED, 2, getRawDeflection(axis) * 1000);
        }
    }

    return avgSetpointSpeed[axis] + boostAmount[axis];
}

FAST_CODE_NOINLINE float applyFfLimit(int axis, float value, float Kp, float currentPidSetpoint) {
    if (axis == FD_ROLL) {
        DEBUG_SET(DEBUG_FF_LIMIT, 0, value);
    }

    if (axis == FD_ROLL) {
        DEBUG_SET(DEBUG_FF_LIMIT, 1, value);
    }

    if (ffMaxRateLimit[axis]) {
        if (fabsf(currentPidSetpoint) <= ffMaxRateLimit[axis]) {
            value = constrainf(value, (-ffMaxRateLimit[axis] - currentPidSetpoint) * Kp, (ffMaxRateLimit[axis] - currentPidSetpoint) * Kp);
        } else {
            value = 0;
        }
    }
    if (axis == FD_ROLL) {
        DEBUG_SET(DEBUG_FF_LIMIT, 2, value);
    }
    return value;
}

bool shouldApplyFfLimits(int axis)
{
    return ffMaxRateLimit[axis] != 0.0f;
}


#endif
