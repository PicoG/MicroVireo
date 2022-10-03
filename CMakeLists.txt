# Pi Pico Vireo Engine
# Pico port started by Derrick Bommarito https://github.com/picog/picovireo

# This build configuration borrows heavily from the design of MicroPython's build system
# https://github.com/micropython/micropython

cmake_minimum_required(VERSION 3.16)

#set(PICOG_PLATFORM "rp2040" CACHE STRING "Sets the type of target to build firmware for.")
#set(PICOG_BOARD "pico" CACHE STRING "Sets the variant of the target to build firmware for.")

### cleanup ccmake / gui config
mark_as_advanced(CMAKE_INSTALL_PREFIX)
mark_as_advanced(CMAKE_BUILD_TYPE)
###

set(PICOG_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

#include all the common config for building picoG firmware
include(./platform/picog/picog.cmake)

set (PICOG_PLATFORM_DIR ${CMAKE_SOURCE_DIR}/platform/${PICOG_PLATFORM})
if(NOT EXISTS ${PICOG_PLATFORM_DIR})
    message(FATAL_ERROR "Invalid PICOG_PLATFORM specified: ${PICOG_PLATFORM}")
endif()

# Set the board directory and check that it exists.
set(PICOG_BOARD_DIR ${PICOG_PLATFORM_DIR}/boards/${PICOG_BOARD})
if(NOT EXISTS ${PICOG_BOARD_DIR}/picog_board.cmake)
    message(FATAL_ERROR "Invalid PICOG_BOARD specified: ${PICOG_BOARD}")
endif()

add_subdirectory(${PICOG_PLATFORM_DIR})

# Include board config which brings in board specific pinouts and peripherals
include(${PICOG_BOARD_DIR}/picog_board.cmake)