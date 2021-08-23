set(BUILD_HEADER "${CMAKE_CURRENT_LIST_DIR}/include/picog/build.h")
set(BUILD_CACHE "${CMAKE_CURRENT_LIST_DIR}/include/picog/version_build.txt")

message(${BUILD_HEADER})

if (EXISTS ${BUILD_CACHE})
    file(READ ${BUILD_CACHE} BUILD_VALUE)
else ()
    set(BUILD_VALUE "1")
endif()

file(WRITE ${BUILD_HEADER} "#ifndef build_h_\n#define build_h_\n#define PICOG_VER_BLD ${BUILD_VALUE}\n#endif //build_h_")

math(EXPR NEXT_VALUE "${BUILD_VALUE}+1")
file(WRITE ${BUILD_CACHE} "${NEXT_VALUE}")