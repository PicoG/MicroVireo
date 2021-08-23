# Pi Pico Vireo Engine
# Pico port started by Derrick Bommarito https://github.com/illuminated-g/VireoSDK

# This build configuration borrows heavily from the design of MicroPython's build system
# https://github.com/micropython/micropython

#Match the same minimum version specified by pico-sdk
cmake_minimum_required(VERSION 3.13)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

if(NOT VIREO_PLATFORM)
    set(VIREO_PLATFORM rp2040)
endif()

# Default vireo board to Pico if not set
if(NOT VIREO_BOARD)
    set(VIREO_BOARD pico)
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

include (../picog/version_build.cmake)

set(PICOG_TARGET "picoG_${VIREO_PLATFORM}_${VIREO_BOARD}_${PICOG_VERSION}")

project(Vireo)

pico_sdk_init()

add_executable(${PICOG_TARGET})

picog_add_version(${PICOG_TARGET})

picog_add_version(${PICOG_TARGET})

# Creates UF2 file and other debugging outputs
pico_add_extra_outputs(${PICOG_TARGET})

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
    pico_enable_stdio_usb(${PICOG_TARGET} 1)
    pico_enable_stdio_uart(${PICOG_TARGET} 0)
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

target_compile_definitions(${PICOG_TARGET}
    PUBLIC __rp2040__ #identify platform within code
    PUBLIC DEBUG_RP=0 #Turn on stdout debugging and tracing of the engine
    PUBLIC DEBUG_MEM=0
)

target_sources(${PICOG_TARGET} PRIVATE
    main.cpp
    io/pico_gpio.cpp
    io/pico_persist.cpp
    io/pico_io.cpp
    ${VIREO_SOURCE_CORE}
    ${VIREO_SOURCE_IO}
)

target_include_directories(${PICOG_TARGET}
    PUBLIC ${VIREO_INCLUDE}
    PUBLIC ../picog/include
)

# These are the components we're using from the pico-sdk
set(PICO_SDK_COMPONENTS
    pico_stdlib
)

target_link_libraries(${PICOG_TARGET}
    ${PICO_SDK_COMPONENTS}
)

#Enable features and types for Vireo, eventually migrate to macros provided in Vireo base cmake
# which will also allow the vireo build to customize which source files get built.
target_compile_definitions(${PICOG_TARGET}
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