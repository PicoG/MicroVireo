#include "TypeDefiner.h"
#include "Instruction.h"

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#ifndef __rp2040__
#error pico_i2c.cpp should only be included in Pico-SDK platform targets
#endif

namespace Vireo {

i2c_inst_t *i2c[2] = { i2c0, i2c1 };

VIREO_FUNCTION_SIGNATURE3(I2CInit, Int32, UInt32, UInt32) {

    //initialize specific I2C bus index at the specified speed and return the actual baud
    _Param(2) = i2c_init(i2c[_Param(0)], _Param(1));

    return _NextInstruction();
}

VIREO_FUNCTION_SIGNATURE5(I2CReadBlocking, Int32, UInt8, Int32, Boolean, StringRef) {
    int bus = _Param(0);
    uint8_t addr = _Param(1);
    uint len = _Param(2);
    bool nostop = _Param(3);
    StringRef out = _Param(4);

    Utf8Char buf[255];


    int read = i2c_read_blocking(i2c[bus],addr, buf, len, nostop);

    out->Replace1D(0, read, buf, true);

    return _NextInstruction();
}

VIREO_FUNCTION_SIGNATURE5(I2CWriteBlocking, Int32, UInt8, StringRef, Boolean, Int32) {
    int bus = _Param(0);
    uint8_t addr = _Param(1);
    StringRef src = _Param(2);
    int len = src->Length();
    bool nostop = _Param(3);

    int written = i2c_write_blocking(i2c[bus], addr, src->Begin(), len, nostop);
    _Param(4) = written;

    return _NextInstruction();
}

DEFINE_VIREO_BEGIN(PicoI2C)
    DEFINE_VIREO_FUNCTION(I2CInit, "p(i(Int32) i(UInt32) o(UInt32))")
    DEFINE_VIREO_FUNCTION(I2CReadBlocking, "p(i(Int32) i(UInt8) i(Int32) i(Boolean) o(String))")
    DEFINE_VIREO_FUNCTION(I2CWriteBlocking, "p(i(Int32) i(UInt8) i(String) i(Boolean) o(Int32))")
DEFINE_VIREO_END()
}