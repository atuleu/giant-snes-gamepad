set(SRC_FILES main.cpp)

set(HDR_FILES )

include_directories(${NCURSES_INCLUDE_DIRS} ${USB_INCLUDE_DIRS})


add_executable(giant-snes-gamepad-interface ${SRC_FILES} ${HDR_FILES})

target_link_libraries(giant-snes-gamepad-interface ${CURSES_LIBRARIES} ${USB_LIBRARIES})

