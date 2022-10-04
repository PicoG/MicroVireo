#include "TypeDefiner.h"
#include "Instruction.h"

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#ifndef __rp2040__
#error pico_i2c.cpp should only be included in Pico-SDK platform targets
#endif

#include "picog/i2c.h"

namespace Vireo {

i2c_inst_t *i2c[2] = { i2c0, i2c1 };

PICOG_INSTRUCTION(I2CInit) {
    //initialize specific I2C bus index at the specified speed and return the actual baud
    _Param(ActualBaud) = i2c_init(i2c[_Param(Bus)], _Param(Baud));

    return _NextInstruction();
}

PICOG_INSTRUCTION(I2CReadBlocking) {
    Utf8Char buf[255];

    int read = i2c_read_blocking(i2c[_Param(Bus)],_Param(Address), buf, _Param(Count), _Param(NoStop));

    _Param(Data)->Replace1D(0, read, buf, true);

    return _NextInstruction();
}

PICOG_INSTRUCTION(I2CWriteBlocking) {

    _Param(Count) = i2c_write_blocking(i2c[_Param(Bus)], _Param(Address), _Param(Data)->Begin(), _Param(Data)->Length(), _Param(NoStop));

    return _NextInstruction();
}

REGISTER_PICOG_I2C()

}