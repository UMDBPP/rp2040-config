cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(config-example C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(config-example 
    name.cpp
    ../../config.cpp
    ../../MB85RS1MT.cpp
)

pico_enable_stdio_usb(config-example 1)

pico_add_extra_outputs(config-example)

target_link_libraries(config-example pico_stdlib hardware_spi hardware_gpio)