################################################################################
# Initialize submodules
################################################################################

find_program(GIT_EXE git)
if(NOT GIT_EXE)
	message(FATAL_ERROR "I need git to configire third party software")
endif(NOT GIT_EXE)

message(STATUS "Checking Third Party Sources")
execute_process(COMMAND ${GIT_EXE} submodule init
                COMMAND ${GIT_EXE} submodule update
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../)

# Sets the toolchain of this porject : an Arduino
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../arduino-cmake/cmake/ArduinoToolchain.cmake)

project(GiantSnesGamepadFw C CXX)

cmake_minimum_required(VERSION 2.8)


################################################################################
# Options for flashing
################################################################################


if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
	message(STATUS "Coucou")
	set(SERIAL_PORT_DEFAULT "/dev/ttyACM0")
	set(SERIAL_PROG_DEFAULT "picocom")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
	set(SERIAL_PORT_DEFAULT "/dev/tty.usbmodem001")
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

if(NOT ARDUINO_SERIAL_EXEC)
	message(FATAL_ERROR "Please set ARDUINO_SERIAL_PROG")
endif(NOT ARDUINO_SERIAL_EXEC)








add_subdirectory(src)