cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(test
	test.c
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
	key.c
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
)

#This will enable output via USB and UART
pico_enable_stdio_usb(test 1)
pico_enable_stdio_uart(test 1)

pico_add_extra_outputs(test)

target_link_libraries(test
	pico_stdlib
	hardware_i2c
)
