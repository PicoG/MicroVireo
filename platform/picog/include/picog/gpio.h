#ifndef gpio_h_
#define gpio_h_

#include "Instruction.h"

#include "picog.h"

namespace Vireo {

PICOG_PARAMS(GpioInit) {
    _ParamDef(UInt32, Pin);
    NEXT_INSTRUCTION_METHOD()
};

PICOG_PARAMS(GpioSetFunction) {
    _ParamDef(UInt32, Pin);
    _ParamDef(UInt8, Func);
    NEXT_INSTRUCTION_METHOD()
};

PICOG_PARAMS(GpioSetOutput) {
    _ParamDef(UInt32, Pin);
    _ParamDef(Boolean, Out);
    NEXT_INSTRUCTION_METHOD()
};

PICOG_PARAMS(GpioSetPulls) {
    _ParamDef(UInt32, Pin);
    _ParamDef(Boolean, Up);
    _ParamDef(Boolean, Down);
    NEXT_INSTRUCTION_METHOD()
};

PICOG_PARAMS(GpioRead) {
    _ParamDef(UInt32, Pin);
    _ParamDef(Boolean, Value);
    NEXT_INSTRUCTION_METHOD()
};

PICOG_PARAMS(GpioWrite) {
    _ParamDef(UInt32, Pin);
    _ParamDef(Boolean, Value);
    NEXT_INSTRUCTION_METHOD()
};

#define REGISTER_PICOG_GPIO() \
DEFINE_VIREO_BEGIN(PicoG_GPIO) \
    DEFINE_VIREO_FUNCTION(GpioInit, "p(i(UInt32))") \
    DEFINE_VIREO_FUNCTION(GpioSetFunction, "p(i(UInt32) i(UInt8))") \
    DEFINE_VIREO_FUNCTION(GpioSetOutput, "p(i(UInt32) i(Boolean))") \
    DEFINE_VIREO_FUNCTION(GpioSetPulls, "p(i(UInt32) i(Boolean) i(Boolean))") \
    DEFINE_VIREO_FUNCTION(GpioRead, "p(i(UInt32) o(Boolean))") \
    DEFINE_VIREO_FUNCTION(GpioWrite, "p(i(UInt32) i(Boolean))") \
DEFINE_VIREO_END()

} //namespace Vireo

#endif //gpio_h_