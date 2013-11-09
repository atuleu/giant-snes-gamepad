set(SRC_FILES blink.cpp )

generate_arduino_firmware(giant-snes-gamepad-fw
                          BOARD micro
						  SRCS ${SRC_FILES}
						  PORT ${ARDUINO_SERIAL_PORT}
						  SERIAL ${ARDUINO_SERIAL_EXEC} ${ARDUINO_SERIAL_PORT} -b 9600 -l )

