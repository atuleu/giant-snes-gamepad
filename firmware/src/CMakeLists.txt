set(SRC_FILES main.c )



#Append Lufa dependencies
set(LUFA_ROOT ${PROJECT_SOURCE_DIR}/../lufa)

list(APPEND SRC_FILES ${LUFA_ROOT}/LUFA/Drivers/USB/Class/Device/HIDClassDevice.c)

add_definitions("-std=c99" "-DF_USB=16000000")

include_directories(${LUFA_ROOT})

generate_avr_firmware(giant-snes-gamepad-fw
                      BOARD micro
                      SRCS ${SRC_FILES}
                      PORT ${ARDUINO_SERIAL_PORT}
                      SERIAL ${ARDUINO_SERIAL_EXEC} ${ARDUINO_SERIAL_PORT} -b 9600 -l )

