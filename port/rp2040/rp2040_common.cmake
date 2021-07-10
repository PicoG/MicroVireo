# We expect RP2040_STDIO to be set to 'usb', 'uart', or 'none'
# If not defined we default to usb.
# Can be set to uart to allow using usb port for other tasks
# and requires setting additional RP2040_STDIO_UART_XX values below
if (NOT DEFINED RP2040_STDIO)
    set(RP2040_STDIO usb)
    message(DEBUG "No STDIO mapping specified, defaulting to usb")
endif ()

#Sets the driver to use for stdio
if (RP2040_STDIO EQUAL "usb")
    set(RP2040_STDIO_MAPPING pico_stdio_usb)
elseif (RP2040_STDIO EQUAL "uart")
    set(RP2040_STDIO_MAPPING pico_stdio_uart)

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