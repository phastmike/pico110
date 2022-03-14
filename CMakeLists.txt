cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(pico110_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(pico110
	main.c
	eeprom.c
	m110.c
	tm1638.c
	tune_step.c
	i2c0.c
	ctcss.c
	radio_channel.c
	memory_channel.c
	radio.c
	hmi.c
	hmi_key.c
	view_controller.c
	vc_brightness.c
	vc_tune_step.c
	vc_timeout.c
	vc_rekey.c
	vc_txadmit.c
	vc_freq.c
	vc_dup.c
	vc_enc.c
	vc_dec.c
	vc_version.c
	vc_shift.c
	vc_apo.c
	vc_pll_na.c
	vc_intro.c
)

#This will enable output via USB and UART
pico_enable_stdio_usb(pico110 1)
pico_enable_stdio_uart(pico110 1)

pico_add_extra_outputs(pico110)

target_link_libraries(pico110
	pico_stdlib
	hardware_i2c
)
