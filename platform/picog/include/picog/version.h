#ifndef version_h_
#define version_h_

//Edit these PICOG_VERSION_X defines to set the static portion of the version number.
//When changing the version number, this should be the only file added to a version bumping commit.

#define PICOG_VER_MAJ 0
#define PICOG_VER_MIN 0
#define PICOG_VER_FIX   1

#include "build.h"

#define PICOG_VER_SEP .

#define _PICOG_STR(x) #x
#define PICOG_STR(x) _PICOG_STR(x)

#define _PICOG_CONCAT(x, y) x##y
#define PICOG_CONCAT(x, y) _PICOG_CONCAT(x, y)

#define PICOG_VERSION PICOG_STR(PICOG_CONCAT(PICOG_VER_MAJ, PICOG_CONCAT(PICOG_VER_SEP, PICOG_CONCAT(PICOG_VER_MIN, PICOG_CONCAT(PICOG_VER_SEP, PICOG_CONCAT(PICOG_VER_FIX, PICOG_CONCAT(PICOG_VER_SEP, PICOG_VER_BLD)))))))

#endif // version_h_