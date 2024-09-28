cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(pico110_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(pico110
	src/main.c
	src/eeprom.c
	src/m110.c
	src/tm1638.c
	src/tune_step.c
	src/i2c0.c
	src/ctcss.c
	src/radio_channel.c
	src/memory_channel.c
	src/radio.c
	src/hmi.c
	src/hmi_key.c
	src/view_controller.c
	src/vc_brightness.c
	src/vc_tune_step.c
	src/vc_timeout.c
	src/vc_rekey.c
	src/vc_txadmit.c
	src/vc_freq.c
	src/vc_dup.c
	src/vc_enc.c
	src/vc_dec.c
	src/vc_version.c
	src/vc_shift.c
	src/vc_apo.c
	src/vc_pll_na.c
	src/vc_intro.c
	src/memory.c
)

#This will enable output via USB and UART
pico_enable_stdio_usb(pico110 1)
pico_enable_stdio_uart(pico110 1)

pico_add_extra_outputs(pico110)

target_link_libraries(pico110
	pico_stdlib
	hardware_i2c
)
