cmake_minimum_required(VERSION 4.1)
project(Analog_Sensor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(Analog_Sensor src/main.cpp
        include/communication.hpp)
target_link_libraries(Analog_Sensor
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)


