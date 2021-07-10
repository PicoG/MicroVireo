# Pi Pico Vireo Engine
# Pico port started by Derrick Bommarito https://github.com/illuminated-g/VireoSDK

# This build configuration borrows heavily from the design of MicroPython's build system
# https://github.com/micropython/micropython

cmake_minimum_required(VERSION 3.13)

# Set build type to reduce firmware size
# Used by pico-sdk
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE MinSizeRel)
endif()

#determine vireo source root
get_filename_component(VIREO_DIR "../../source" ABSOLUTE)

# Default rp2040 board to Pico if not set
if(NOT VIREO_BOARD)
    set(VIREO_BOARD PICO)
endif()

# Set the board directory and check that it exists.
if(NOT VIREO_BOARD_DIR)
    set(VIREO_BOARD_DIR ${CMAKE_SOURCE_DIR}/boards/${VIREO_BOARD})
endif()
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/vireoboard.cmake)
    message(FATAL_ERROR "Invalid VIREO_BOARD specified: ${RP2040_BOARD}")
endif()

# Include board config
include(${VIREO_BOARD_DIR}/vireoboard.cmake) 

include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(Vireo)

pico_sdk_init()

add_executable(vireo )

pico_add_extra_outputs(vireo)