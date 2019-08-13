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

#if defined(USE_DSHOT) && defined(USE_DSHOT_TELEMETRY)

void decodeBitBangTelemetryPacket(
    uint32_t buffer[], uint32_t count, uint32_t values[], uint32_t bitMasks[], uint32_t bitCount);

uint32_t convertTelemetryValueToRpmDiv100(uint32_t value);

#endif

