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

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

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

# Brings in the Pi Pico SDK, needed before declaring project
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(Vireo)

pico_sdk_init()

add_executable(vireo)

# Creates UF2 file and other debugging outputs
pico_add_extra_outputs(vireo)

message("\nConfiguring Vireo Engine build\n")

# We expect RP2040_STDIO to be set to 'usb', 'uart', or 'none'
# If not defined we default to usb.
# Can be set to uart to allow using usb port for other tasks
# and requires setting additional RP2040_STDIO_UART_XX values below
if (NOT DEFINED RP2040_STDIO)
    set(RP2040_STDIO "usb")
    message("No STDIO mapping specified, defaulting to usb")
endif ()

message("STDIO configured over ${RP2040_STDIO}")

#Sets the driver to use for stdio
if (RP2040_STDIO STREQUAL "usb")
    pico_enable_stdio_usb(vireo 1)
    pico_enable_stdio_uart(vireo 0)
elseif (RP2040_STDIO STREQUAL "uart")
    set(RP2040_STDIO_MAPPING "pico_stdio_uart")

    # The following VIREO_STDIO_UART_XX values configure the port when pico_stdio_uart is used above
    # Default is pins on the bottom right of the Pico pinout diagram, similar to small arduino board RX/TX layout
    if (NOT DEFINED RP2040_STDIO_UART_PORT)
        set(RP2040_STDIO_UART_PORT uart0)
    endif ()

    if (NOT DEFINED RP2040_STDIO_UART_BAUD)
        set(RP2040_STDIO_UART_BAUD 115200)
    endif ()

    # Note: RX and TX values can be set to -1 to disable that stdio direction
    if (NOT DEFINED RP2040_STDIO_UART_RX)
        set(RP2040_STDIO_UART_RX 21)
    endif()

    if (NOT DEFINED RP2040_STDIO_UART_TX)
        set(RP2040_STDIO_UART_TX 22)
    endif ()
else()
    unset(RP2040_STDIO_MAPPING)
endif ()

message("STDIO Mapping used is ${RP2040_STDIO_MAPPING}")

target_compile_definitions(vireo
    PUBLIC __rp2040__ #identify platform within code
    PUBLIC DEBUG_RP=1 #Turn on stdout debugging and tracing of the engine
    PUBLIC VIREO_MINI=1
)

target_sources(vireo PRIVATE
    micromain.cpp
    ${VIREO_SOURCE_CORE}
)

target_include_directories(vireo PUBLIC
    ${VIREO_INCLUDE}
)

# These are the components we're using from the pico-sdk
set(PICO_SDK_COMPONENTS
    pico_stdlib
)

target_link_libraries(vireo
    ${PICO_SDK_COMPONENTS}
)