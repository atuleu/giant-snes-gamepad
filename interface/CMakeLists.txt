project(GiantSnesGamepadInterface C CXX)

cmake_minimum_required(VERSION 2.8)

set(CURSES_NEED_NCURSES_H TRUE)
find_package(Curses REQUIRED)
find_package(PkgConfig REQUIRED)


find_path(NCURSES_INCLUDE_DIRS ncurses.h PATH_SUFFIXES ncurses)
if(NOT NCURSES_INCLUDE_DIRS)
	message(FATAL_ERROR "Found NCurses library but not the header o_O")
endif(NOT NCURSES_INCLUDE_DIRS)

pkg_check_modules(USB libusb-1.0 REQUIRED)

#we use C++11
add_definitions("--std=c++11")

add_subdirectory(src)




	
