set(SRC_FILES Descriptor.c
              main.c 
              USB.c
              Gamepad.c
              Systime.c
              )
set(HDR_FILES Descriptor.h
	          USB.h
	          Gamepad.h
	          Systime.h
	          )


#Append Lufa dependencies
set(LUFA_ROOT ${PROJECT_SOURCE_DIR}/../lufa)
set(LUFA_CORE_ROOT ${LUFA_ROOT}/LUFA/Drivers/USB/Core)


add_definitions("-std=c99" # LUFA is c99
                "-DF_USB=16000000" # Arduino's micro board
                )

include_directories(${LUFA_ROOT})
set(ARCH AVR8)
set(LUFA_CORE_SRCS ConfigDescriptors.c
	               DeviceStandardReq.c
	               Events.c
	               # HostStandardReq.c
	               USBTask.c
	               ${ARCH}/Device_${ARCH}.c
	               ${ARCH}/Endpoint_${ARCH}.c
	               ${ARCH}/EndpointStream_${ARCH}.c
	               # ${ARCH}/Host_${ARCH}.c
	               # ${ARCH}/Pipe_${ARCH}.c
	               # ${ARCH}/PipeStream_${ARCH}.c
	               ${ARCH}/USBController_${ARCH}.c
	               ${ARCH}/USBInterrupt_${ARCH}.c
	               # Class/Common/HIDParser.c
	               )

set(LUFA_CORE_HDRS LUFA/USB/USB.h)

foreach(f ${LUFA_CORE_SRCS})
	list(APPEND LUFA_CORE_SRCS_EXPANDED ${LUFA_CORE_ROOT}/${f})
endforeach(f ${LUFA_CORE_SRCS})

generate_avr_library(lufa-core-device
                     BOARD micro
                     SRCS ${LUFA_CORE_SRCS_EXPANDED})

set(giant-snes-gamepad-fw_LIBS lufa-core-device)
generate_avr_firmware(giant-snes-gamepad-fw
                      BOARD micro
                      SRCS ${SRC_FILES}
                      HDRS ${HDR_FILES}
                      LIBS lufa-core-device
                      PORT ${ARDUINO_SERIAL_PORT}
                      SERIAL ${ARDUINO_SERIAL_EXEC} ${ARDUINO_SERIAL_PORT} -b 9600 -l 
                      )

