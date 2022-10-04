#ifndef i2c_h_
#define i2c_h_

#include "Instruction.h"

#include "picog.h"

namespace Vireo {

PICOG_PARAMS(I2CInit) {
    _ParamDef(Int32, Bus);
    _ParamDef(UInt32, Baud);
    _ParamDef(UInt32, ActualBaud);
    NEXT_INSTRUCTION_METHOD()
};

PICOG_PARAMS(I2CReadBlocking) {
    _ParamDef(Int32, Bus);
    _ParamDef(UInt8, Address);
    _ParamDef(Int32, Count);
    _ParamDef(Boolean, NoStop);
    _ParamDef(StringRef, Data);
    NEXT_INSTRUCTION_METHOD()
};

PICOG_PARAMS(I2CWriteBlocking) {
    _ParamDef(Int32, Bus);
    _ParamDef(UInt8, Address);
    _ParamDef(StringRef, Data);
    _ParamDef(Boolean, NoStop);
    _ParamDef(Int32, Count);
    NEXT_INSTRUCTION_METHOD()
};

#define REGISTER_PICOG_I2C() \
DEFINE_VIREO_BEGIN(PicoI2C)\
    DEFINE_VIREO_FUNCTION(I2CInit, "p(i(Int32) i(UInt32) o(UInt32))")\
    DEFINE_VIREO_FUNCTION(I2CReadBlocking, "p(i(Int32) i(UInt8) i(Int32) i(Boolean) o(String))")\
    DEFINE_VIREO_FUNCTION(I2CWriteBlocking, "p(i(Int32) i(UInt8) i(String) i(Boolean) o(Int32))")\
DEFINE_VIREO_END()

} //namespace Vireo

#endif //i2c_h_