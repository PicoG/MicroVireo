//Get Platform.h from DataTypes.h 
#include "DataTypes.h"

#include <pico/stdio.h>

namespace Vireo {

int PlatformIO::_getchar_timeout_us(uint32_t timeout_us) {
    int c;
#ifdef __rp2040__
    c = getchar_timeout_us(timeout_us);
    if (c == PICO_ERROR_TIMEOUT) {
        return -1;
    }
#else
    #error No implementation for _getchar_timeout_us on current platform
#endif

    return c;
}

} //namespace Vireo