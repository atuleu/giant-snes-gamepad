set(SRC_FILES main.cpp
	          MainWindow.cpp
	          Gamepad.cpp
	          common.cpp
	          CellViewer.cpp)

set(HDR_FILES MainWindow.h
	          Gamepad.h
	          common.h
	          CellViewer.h
	          ${PROJECT_SOURCE_DIR}/../common/communication.h)

set(UI_RAW_FILES MainWindow.ui CellViewer.ui)

set(TO_MOC_FILES MainWindow.h CellViewer.h)

include(${QT_USE_FILE})


include_directories(${NCURSES_INCLUDE_DIRS} ${DEPS_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR})

if(APPLE)
	add_definitions("-DLIBUSB_DARWIN_WORKAROUND")
endif(APPLE)


qt4_wrap_cpp(MOC_FILES ${TO_MOC_FILES})

qt4_wrap_ui(UI_FILES ${UI_RAW_FILES})

add_executable(giant-snes-gamepad-interface ${SRC_FILES} ${HDR_FILES} ${UI_FILES} ${MOC_FILES})

target_link_libraries(giant-snes-gamepad-interface ${CURSES_LIBRARIES} ${DEPS_LIBRARIES} ${QT_LIBRARIES})

