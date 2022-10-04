#ifndef picog_h_
#define picog_h_

#include "picog/version.h"

#define PICOG_PLATFORM PICOG_STR(_PICOG_PLATFORM)
#define PICOG_BOARD PICOG_STR(_PICOG_BOARD)

#define PICOG_INSTRUCTION(_name_) VIREO_FUNCTION_SIGNATURET(_name_, PicoG_ ## _name_ ## _Params)

#define PICOG_PARAMS(_name_) struct PicoG_ ## _name_ ## _Params : InstructionCore

#endif //picog_h_