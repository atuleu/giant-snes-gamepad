project(GiantSnesGamepadInterface C CXX)

cmake_minimum_required(VERSION 2.8)


find_package(PkgConfig REQUIRED)


find_package(Qt REQUIRED QtCore QtGui)

pkg_check_modules(DEPS libusb-1.0 libglog REQUIRED)

#we use C++11
add_definitions("--std=c++11")

add_subdirectory(src)




	
