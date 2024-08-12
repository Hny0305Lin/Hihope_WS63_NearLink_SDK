#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLINKY)
    add_subdirectory_if_exist(blinky)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BUTTON)
    add_subdirectory_if_exist(button)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_TASKS)
    add_subdirectory_if_exist(tasks)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_TCXO)
    add_subdirectory_if_exist(tcxo)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_I2C)
    add_subdirectory_if_exist(i2c)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_PINCTRL)
    add_subdirectory_if_exist(pinctrl)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SPI)
    add_subdirectory_if_exist(spi)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SYSTICK)
    add_subdirectory_if_exist(systick)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_WDT)
    add_subdirectory_if_exist(watchdog)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_PWM)
    add_subdirectory_if_exist(pwm)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_UART)
    add_subdirectory_if_exist(uart)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_ADC)
    add_subdirectory_if_exist(adc)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_TIMER)
    add_subdirectory_if_exist(timer)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)