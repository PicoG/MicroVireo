# Pi Pico Vireo Engine
# Pico port started by Derrick Bommarito https://github.com/illuminated-g/VireoSDK

# This build configuration borrows heavily from the design of MicroPython's build system
# https://github.com/micropython/micropython

#Match the same minimum version specified by pico-sdk
cmake_minimum_required(VERSION 3.13)

# Set build type to reduce firmware size
# Used by pico-sdk
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE MinSizeRel)
endif()

#See if PICK_SDK_PATH already defined by environment, otherwise set it to default
if (DEFINED ENV{PICO_SDK_PATH})
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
else ()
    #default to looking for pico-sdk as a sibling of the Vireo folder
    get_filename_component(PICO_SDK_PATH "../../../pico-sdk" ABSOLUTE)
    if (NOT EXISTS ${PICO_SDK_PATH}/pico_sdk_init.cmake)
        message(FATAL_ERROR "PICO_SDK_PATH variable not set in environment.")
    endif()
endif()

#determine vireo source root
get_filename_component(VIREO_DIR "../../source" ABSOLUTE)

# Default vireo board to Pico if not set
if(NOT VIREO_BOARD)
    set(VIREO_BOARD PICO)
endif()

# Set the board directory and check that it exists.
if(NOT VIREO_BOARD_DIR)
    set(VIREO_BOARD_DIR ${CMAKE_SOURCE_DIR}/boards/${VIREO_BOARD})
endif()
if(NOT EXISTS ${VIREO_BOARD_DIR}/vireoboard.cmake)
    message(FATAL_ERROR "Invalid VIREO_BOARD specified: ${VIREO_BOARD}")
endif()

# Include board config which brings in board specific pinouts and peripherals
include(${VIREO_BOARD_DIR}/vireoboard.cmake) 


# Include the configuration for vireo and the main vireo source
include(vireo_config.cmake)
include(${VIREO_DIR}/vireo.cmake)

# Brings in the Pi Pico SDK
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(Vireo)

pico_sdk_init()

add_executable(vireo)

# Creates UF2 file and other debugging outputs
pico_add_extra_outputs(vireo)

# These are the components we're using from the pico-sdk
set(PICO_SDK_COMPONENTS
    pico_stdlib
)
