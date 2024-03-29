/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2016, Alexey Kramarenko
    All rights reserved.
*/

#ifndef STM32_CONFIG_H
#define STM32_CONFIG_H

//---------------------- fast drivers definitions -----------------------------------
#define STM32_ADC_DRIVER                        0
#define STM32_DAC_DRIVER                        0
#define STM32_WDT_DRIVER                        0
#define STM32_EEP_DRIVER                        1
#define STM32_FLASH_DRIVER                      0
#define STM32_I2C_DRIVER                        0
#define STM32_SPI_DRIVER                        0
#define STM32_UART_DRIVER                       1
#define STM32_RTC_DRIVER                        0
#define STM32_USB_DRIVER                        1
//------------------------------ CORE ------------------------------------------------
//Sizeof CORE process stack. Adjust, if monolith UART/USB/Analog/etc is used
#define STM32_CORE_PROCESS_SIZE                 780

//disable only for power saving if no EXTI or remap is used
#define SYSCFG_ENABLED                          1
//stupid F1 series only. Remap mask. See datasheet
#define STM32F1_MAPR                            (AFIO_MAPR_USART2_REMAP)
//------------------------------- UART -----------------------------------------------
//in any half duplex mode
#define STM32_UART_DISABLE_ECHO                 0
//------------------------------ POWER -----------------------------------------------
//save few bytes here
#define STM32_DECODE_RESET                      0
//0 meaning HSI. If not defined, 25MHz will be defined by default by ST lib
#define HSE_VALUE                               0
// HSE_RTC_DIV used for getting 1 MHz (or lower) clock frequency for RTC on SMT32L1.
#define HSE_RTC_DIV                             0

#define HSE_BYPASS                              0
//0 meaning HSE
#define LSE_VALUE                               0

//STM32L0 || STM32L
#define MSI_RANGE                               0

#define PLL_MUL                                 4
#define PLL_DIV                                 2

//STM32F10X_CL only
// use PLL2 as clock source for main PLL. Set to 0 to disable
#define PLL2_DIV                                0
#define PLL2_MUL                                0

//STM32F2, STM32F4
#define PLL_M                                   0
#define PLL_N                                   0
#define PLL_P                                   0

#define STANDBY_WKUP                            0
//------------------------------ TIMER -----------------------------------------------
#define HPET_TIMER                              TIM_21
//only required if no STM32_RTC_DRIVER is set
#define SECOND_PULSE_TIMER                      TIM_22
//disable to save few bytes
#define TIMER_IO                                1
//------------------------------- ADC ------------------------------------------------
//In L0 series - select HSI16 as clock source
#define STM32_ADC_ASYNCRONOUS_CLOCK             0
// Avg Slope, refer to datasheet
#define AVG_SLOPE                               4300
// temp at 25C in mV, refer to datasheet
#define V25_MV                                  1400

//------------------------------- DAC ------------------------------------------------
#define DAC_BOFF                                0

//DAC streaming support with DMA. Can be disabled for flash saving
#define DAC_STREAM                              0
//For F1 only
#define DAC_DUAL_CHANNEL                        0
//Decrease for memory saving, increase if you have data underflow.
//Generally you will need fifo enough to hold samples at least for 30-50us, depending on core frequency.
//Size in samples
#define DAC_DMA_FIFO_SIZE                       128
//Turn on for DAC data underflow debug.
#define DAC_DEBUG                               0

//------------------------------- USB ------------------------------------------------
//Maximum packet size for USB.
//Must be 8 for Low-speed devices
//Full speed: 64 if no isochronous transfers, else  1024
//High speed(STM32F2+): 64 if no high-speed bulk transfers, 512 in other case. 1024 in case of isochronous or high-speed interrupts
#define STM32_USB_MPS                           64
//------------------------------- WDT ------------------------------------------------
//if set by STM32 Option Bits, WDT is started by hardware on power-up
#define HARDWARE_WATCHDOG                       0
//------------------------------- ETH -----------------------------------------------
#define STM32_ETH_PROCESS_SIZE                  512

#endif // STM32_CONFIG_H
