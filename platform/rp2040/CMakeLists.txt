# Pi Pico Vireo Engine
# Pico port started by Derrick Bommarito https://github.com/illuminated-g/VireoSDK

# This build configuration borrows heavily from the design of MicroPython's build system
# https://github.com/micropython/micropython

#Match the same minimum version specified by pico-sdk
cmake_minimum_required(VERSION 3.13)

if(NOT VIREO_PLATFORM)
    set(VIREO_PLATFORM rp2040)
endif()

# Default vireo board to Pico if not set
if(NOT VIREO_BOARD)
    set(VIREO_BOARD sfpromicro)
endif()

#include all the common config for building picoG firmware
include (../picog/picog.cmake)

# Brings in the Pi Pico SDK, needed before declaring project
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(picoG)
pico_sdk_init()

create_picog_build(PICO_TARGET)

# Creates UF2 file and other debugging outputs
pico_add_extra_outputs(${PICO_TARGET})

message("\nConfiguring rp2040 Engine build\n")

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
    pico_enable_stdio_usb(${PICO_TARGET} 1)
    pico_enable_stdio_uart(${PICO_TARGET} 0)
elseif (RP2040_STDIO STREQUAL "uart")

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
endif ()

target_compile_definitions(${PICO_TARGET}
    PUBLIC __rp2040__ #identify platform within code
    PUBLIC DEBUG_RP=0 #Turn on stdout debugging and tracing of the engine
    PUBLIC DEBUG_MEM=0
)

target_sources(${PICO_TARGET} PRIVATE
    main.cpp
    io/pico_gpio.cpp
    io/pico_persist.cpp
    io/pico_io.cpp
)

# These are the components we're using from the pico-sdk
set(PICO_SDK_COMPONENTS
    pico_stdlib
)

target_link_libraries(${PICO_TARGET}
    ${PICO_SDK_COMPONENTS}
)

#Enable features and types for Vireo, eventually migrate to macros provided in Vireo base cmake
# which will also allow the vireo build to customize which source files get built.
target_compile_definitions(${PICO_TARGET}
    PUBLIC VIREO_SKIP_CFG_TYPES=1 #Skip configuration defines in BuildConfig.h
    PUBLIC VIREO_DEBUG_EXEC_PRINT_INSTRS=0

    PUBLIC VIREO_TYPE_UInt32=1
    #PUBLIC VIREO_TYPE_UInt16=1
    PUBLIC VIREO_TYPE_Int32=1
    #PUBLIC VIREO_TYPE_Double=1
    PUBLIC VIREO_TYPE_Events=1
    #PUBLIC VIREO_TYPE_ControlRef=1
    #PUBLIC VIREO_TYPE_JSRefs=1
    #PUBLIC VIREO_TYPE_ArrayND=1
    PUBLIC VIREO_VIA_FORMATTER=1
    PUBLIC VIREO_POSIX_FILEIO=1
    PUBLIC VIREO_TRACK_MALLOC=1

    PUBLIC VIREO_VIA_PERSIST=1 # Provision for persisting VIA source to device for autorun on boot
    #PRIVATE kVireoOS_linuxU=1
)