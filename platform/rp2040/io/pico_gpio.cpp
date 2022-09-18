#include "TypeDefiner.h"
#include "Instruction.h"

#include "pico/stdlib.h"

#ifndef __rp2040__
#error pico_gpio.cpp should only be included in Pico-SDK platform targets
#endif

namespace Vireo {

VIREO_FUNCTION_SIGNATURE1(GpioInit, UInt32) {
    uint pin = _Param(0);

    //fprintf(stdout, "Init %d\n", pin); fflush(stdout);

    gpio_init(pin);

    return _NextInstruction();
}

VIREO_FUNCTION_SIGNATURE2(GpioSetFunction, UInt32, UInt8) {
    uint pin = _Param(0);
    enum gpio_function func = (enum gpio_function)_Param(1);

    gpio_set_function(pin, func);

    return _NextInstruction();
}

VIREO_FUNCTION_SIGNATURE2(GpioSetOutput, UInt32, Boolean) {
    uint pin = _Param(0);
    bool out = _Param(1);

    //fprintf(stdout, "SetOutput %d %d\n", pin, out); fflush(stdout);

    gpio_set_dir(pin, out);

    return _NextInstruction();
}

VIREO_FUNCTION_SIGNATURE3(GpioSetPulls, UInt32, Boolean, Boolean) {
    uint pin = _Param(0);
    bool up = _Param(1);
    bool down = _Param(2);

    gpio_set_pulls(pin, up, down);

    return _NextInstruction();
}

VIREO_FUNCTION_SIGNATURE2(GpioRead, UInt32, Boolean) {
    uint pin = _Param(0);

    _Param(1) = gpio_get(pin);

    return _NextInstruction();
}

VIREO_FUNCTION_SIGNATURE2(GpioWrite, UInt32, Boolean) {
    uint pin = _Param(0);
    bool val = _Param(1);

    //fprintf(stdout, "Write %d %d\n", pin, val); fflush(stdout);

    gpio_put(pin, val);

    return _NextInstruction();
}

DEFINE_VIREO_BEGIN(PicoGPIO)
    DEFINE_VIREO_FUNCTION(GpioInit, "p(i(UInt32))")
    DEFINE_VIREO_FUNCTION(GpioSetFunction, "p(i(UInt32) i(UInt8))")
    DEFINE_VIREO_FUNCTION(GpioSetOutput, "p(i(UInt32) i(Boolean))")
    DEFINE_VIREO_FUNCTION(GpioSetPulls, "p(i(UInt32) i(Boolean) i(Boolean))")
    DEFINE_VIREO_FUNCTION(GpioRead, "p(i(UInt32) o(Boolean))")
    DEFINE_VIREO_FUNCTION(GpioWrite, "p(i(UInt32) i(Boolean))")
DEFINE_VIREO_END()

}