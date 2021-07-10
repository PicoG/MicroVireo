#cmake for Raspberry Pi Pico

#Sets the driver to use for stdio, options are pico_stdio_uart or pico_stdio_uart
set(RP2040_STDIO_DRIVER pico_stdio_usb)

# The following VIREO_STDIO_UART_XX values configure the port when pico_stdio_uart is used above
# Default is pins on the bottom right of the Pico pinout diagram, similar to small arduino board RX/TX layout
set(RP2040_STDIO_UART_PORT uart0)
set(RP2040_STDIO_UART_BAUD 115200)
set(RP2040_STDIO_UART_RX 21)
set(RP2040_STDIO_UART_TX 22)