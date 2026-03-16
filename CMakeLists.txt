cmake_minimum_required(VERSION 4.1)
project(Analog_Sensor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(TEST_EXE_DIR ${CMAKE_SOURCE_DIR}/test)

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Network
        SerialPort
        SerialBus
        REQUIRED)

add_executable(test_communication test/test_communication_main.cpp include/communication.hpp
        "test/test_communication.hpp")
set_target_properties(test_communication PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${TEST_EXE_DIR}
)

add_executable(test_presentation test/test_presentation_main.cpp include/presentation.hpp
        "test/test_presentation.hpp")
set_target_properties(test_presentation PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${TEST_EXE_DIR}
)

add_executable(Analog_Sensor src/main.cpp
        include/communication.hpp
        include/presentation.hpp)

target_link_libraries(Analog_Sensor
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Network
        Qt5::SerialPort
        Qt5::SerialBus
)

target_link_libraries(test_communication
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Network
        Qt5::SerialPort
        Qt5::SerialBus
)

target_link_libraries(test_presentation
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Network
        Qt5::SerialPort
        Qt5::SerialBus
)


