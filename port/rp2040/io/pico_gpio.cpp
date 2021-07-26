#include "TypeDefiner.h"
#include "Instruction.h"

#include "pico/stdlib.h"

#ifndef __rp2040__
#error pico_gpio.cpp should only be included in Pico-SDK platform targets
#endif

namespace Vireo {

VIREO_FUNCTION_SIGNATURE1(GpioInit, UInt32) {
    uint pin = _Param(0);

    gpio_init(pin);

    return _NextInstruction();
}

VIREO_FUNCTION_SIGNATURE2(GpioSetOutput, UInt32, Boolean) {
    uint pin = _Param(0);
    bool out = _Param(1);

    gpio_set_dir(pin, out);

    return _NextInstruction();
}

VIREO_FUNCTION_SIGNATURE2(GpioWrite, UInt32, Boolean) {
    uint pin = _Param(0);
    bool val = _Param(1);

    gpio_put(pin, val);

    return _NextInstruction();
}

DEFINE_VIREO_BEGIN(PicoGPIO)
    DEFINE_VIREO_FUNCTION(GpioInit, "p(i(UInt32))")
    DEFINE_VIREO_FUNCTION(GpioSetOutput, "p(i(UInt32) i(Boolean))")
    DEFINE_VIREO_FUNCTION(GpioWrite, "p(i(UInt32) i(Boolean))")
DEFINE_VIREO_END()

}