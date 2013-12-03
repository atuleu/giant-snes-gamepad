# Sets the toolchain of this porject : an Arduino
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../arduino-cmake/cmake/ArduinoToolchain.cmake)

project(GiantSnesGamepadFw C CXX)

cmake_minimum_required(VERSION 2.8)


################################################################################
# Options for flashing
################################################################################


if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
	set(SERIAL_PORT_DEFAULT "/dev/ttyACM0")
	set(SERIAL_PROG_DEFAULT "picocom")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
	set(SERIAL_PORT_DEFAULT "/dev/tty.usbmodem1411")
	set(SERIAL_PROG_DEFAULT "screen")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
	set(SERIAL_PORT_DEFAULT )
	set(SERIAL_PROG_DEFAULT )
endif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")

set(ARDUINO_SERIAL_PORT ${SERIAL_PORT_DEFAULT} CACHE STRING "Serial Port to use" )
set(ARDUINO_SERIAL_PROG ${SERIAL_PROG_DEFAULT} CACHE STRING "Serial Program to use" )
find_program(ARDUINO_SERIAL_EXEC ${SERIAL_PROG_DEFAULT} )
if(NOT ARDUINO_SERIAL_PORT)
	message(FATAL_ERROR "Please set ARDUINO_SERIAL_PORT")
endif(NOT ARDUINO_SERIAL_PORT)

if(NOT ARDUINO_SERIAL_PROG)
	message(FATAL_ERROR "Please set ARDUINO_SERIAL_PROG")
endif(NOT ARDUINO_SERIAL_PROG)








add_subdirectory(src)