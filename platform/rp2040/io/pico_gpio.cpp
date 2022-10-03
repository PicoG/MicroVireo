#include "TypeDefiner.h"
#include "Instruction.h"

#include "pico/stdlib.h"

#ifndef __rp2040__
#error pico_gpio.cpp should only be included in Pico-SDK platform targets
#endif

#include "picog/gpio.h"

namespace Vireo {

PICOG_INSTRUCTION(GpioInit) {

    gpio_init(_Param(Pin));

    return _NextInstruction();
}

PICOG_INSTRUCTION(GpioSetFunction) {
    enum gpio_function func = (enum gpio_function)_Param(Func);

    gpio_set_function(_Param(Pin), func);

    return _NextInstruction();
}

PICOG_INSTRUCTION(GpioSetOutput) {

    //fprintf(stdout, "SetOutput %d %d\n", pin, out); fflush(stdout);

    gpio_set_dir(_Param(Pin), _Param(Out));

    return _NextInstruction();
}

PICOG_INSTRUCTION(GpioSetPulls) {

    gpio_set_pulls(_Param(Pin), _Param(Up), _Param(Down));

    return _NextInstruction();
}

PICOG_INSTRUCTION(GpioRead) {

    _Param(Value) = gpio_get(_Param(Pin));

    return _NextInstruction();
}

PICOG_INSTRUCTION(GpioWrite) {

    //fprintf(stdout, "Write %d %d\n", pin, val); fflush(stdout);

    gpio_put(_Param(Pin), _Param(Value));

    return _NextInstruction();
}

REGISTER_PICOG_GPIO()

}