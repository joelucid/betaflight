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

#pragma once

#if defined(LFF4)
#define TARGET_BOARD_IDENTIFIER "LFF4"
#endif

#define USBD_PRODUCT_STRING "LightFlightF4"


#define LED0_PIN                PC2

#define USE_EXTI

#define USE_ACC
#define USE_ACC_SPI_MPU6500
#define USE_ACC_SPI_MPU6000

#define USE_GYRO
#define USE_GYRO_SPI_MPU6500
#define USE_GYRO_SPI_MPU6000

#define GYRO_1_CS_PIN           PA4
#define GYRO_1_SPI_INSTANCE     SPI1

// MPU6000 interrupts
#define USE_GYRO_EXTI
#define GYRO_1_EXTI_PIN         PC4
#define USE_MPU_DATA_READY_SIGNAL

#define GYRO_1_ALIGN            ALIGN_DEFAULT
#define ACC_1_ALIGN             ALIGN_DEFAULT

// Dummy defines
#define GYRO_2_SPI_INSTANCE     GYRO_1_SPI_INSTANCE
#define GYRO_2_CS_PIN           NONE
#define GYRO_2_ALIGN            ALIGN_DEFAULT
#define GYRO_2_EXTI_PIN         NONE
#define ACC_2_ALIGN             ALIGN_DEFAULT

// Globally configure flashfs and drivers for various flash chips
#define USE_FLASHFS
#define USE_FLASH_M25P16
#define ENABLE_BLACKBOX_LOGGING_ON_SPIFLASH_BY_DEFAULT
#define FLASH_CS_PIN            PA15
#define FLASH_SPI_INSTANCE      SPI3

#define USE_RX_SPI
#define RX_SPI_INSTANCE         SPI2

#define USE_RX_FRSKY_SPI_D
#define USE_RX_FRSKY_SPI_X
#define USE_RX_SFHSS_SPI
#define DEFAULT_RX_FEATURE      FEATURE_RX_SPI
#define RX_SPI_DEFAULT_PROTOCOL RX_SPI_FRSKY_X
#define USE_RX_FRSKY_SPI_TELEMETRY
#define RX_NSS_PIN               PB12
#define RX_CC2500_SPI_GDO_0_PIN   PC7
#define RX_SPI_LED_PIN            LED0_PIN

#define USE_VCP
// #define USE_USB_DETECT
// #define USB_DETECT_PIN   PC5

#define USE_UART1
#define UART1_RX_PIN            PB7
#define UART1_TX_PIN            PB6

//#define USE_UART3
//#define UART3_RX_PIN            PB11
//#define UART3_TX_PIN            PB10

//#define USE_SOFTSERIAL1
//#define USE_SOFTSERIAL2

#define SERIAL_PORT_COUNT       2 // VCP, USART1

#define USE_SPI
#define USE_SPI_DEVICE_1

#define USE_SPI_DEVICE_2
#define SPI2_SCK_PIN            PB13
#define SPI2_MISO_PIN           PB14
#define SPI2_MOSI_PIN           PB15

#define USE_SPI_DEVICE_3
#define SPI3_SCK_PIN            PC10
#define SPI3_MISO_PIN           PC11
#define SPI3_MOSI_PIN           PC12

#define USE_ADC
#define ADC_INSTANCE            ADC2
#define ADC2_DMA_OPT            1  // DMA 2 Stream 3 Channel 1 (compat default)

#define VBAT_ADC_PIN            PC0  // 11:1 (10K + 1K) divider

#define DEFAULT_VOLTAGE_METER_SOURCE VOLTAGE_METER_ADC
//#define DEFAULT_CURRENT_METER_SOURCE CURRENT_METER_ADC

#define TARGET_IO_PORTA (0xffff & ~(BIT(14)|BIT(13)))
#define TARGET_IO_PORTB (0xffff & ~(BIT(2)))
#define TARGET_IO_PORTC (0xffff & ~(BIT(15)|BIT(14)|BIT(13)))
#define TARGET_IO_PORTD BIT(2)

#define USABLE_TIMER_CHANNEL_COUNT 10
#define USED_TIMERS ( TIM_N(2) | TIM_N(5) | TIM_N(4) )
