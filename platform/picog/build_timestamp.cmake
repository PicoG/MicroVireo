set(BUILD_HEADER "${CMAKE_CURRENT_LIST_DIR}/include/picog/build.h")

string(TIMESTAMP PICOG_BUILD_DATE "%Y%m%d" UTC)
string(TIMESTAMP PICOG_BUILD_TIME "%H%M%S" UTC)
set(PICOG_BUILD_TS "${PICOG_BUILD_DATE}T${PICOG_BUILD_TIME}Z")

message ("\nSetting build timestamp to ${PICOG_BUILD_TS}\n")

file(WRITE ${BUILD_HEADER} "//This file is automatically generated by the build system.\n#ifndef build_h_\n#define build_h_\n\n#define PICOG_VER_TS ${PICOG_BUILD_TS}\n\n#endif //build_h_")