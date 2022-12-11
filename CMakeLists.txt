cmake_minimum_required(VERSION 3.24)
project(DeviceManager)

include_directories(inc/)

set(CMAKE_CXX_STANDARD 17)

add_executable(DeviceManager main.cpp DeviceTable.cpp inc/DeviceTable.h DeviceInfoTable.cpp inc/DeviceInfoTable.h DeviceManager.cpp inc/DeviceManager.h)
