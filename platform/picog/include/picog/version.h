#ifndef version_h_
#define version_h_

//The version number can be editted in ../version_build.cmake

#include "common.h"
#include "build.h"

#define PICOG_VER_SEP .

#define PICOG_VERSION_TS PICOG_STR(PICOG_VER_TS)

#define PICOG_VERSION PICOG_STR(PICOG_CONCAT(PICOG_VER_MAJOR, PICOG_CONCAT(PICOG_VER_SEP, PICOG_CONCAT(PICOG_VER_MINOR, PICOG_CONCAT(PICOG_VER_SEP, PICOG_VER_PATCH)))))

#endif // version_h_