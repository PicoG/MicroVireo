#edit the firmware version here
set(PICOG_VER_MAJOR "0")
set(PICOG_VER_MINOR "0")
set(PICOG_VER_PATCH "2")

set(BUILD_HEADER "${CMAKE_CURRENT_LIST_DIR}/include/picog/build.h")
set(BUILD_CACHE "${CMAKE_CURRENT_LIST_DIR}/include/picog/version_build.txt")

get_filename_component(BUILD_INC_PATH "${CMAKE_CURRENT_LIST_DIR}/build_increment.cmake" ABSOLUTE)

set(PICOG_VERSION "${PICOG_VER_MAJOR}.${PICOG_VER_MINOR}.${PICOG_VER_PATCH}")

add_custom_target(picoG_Version)

add_custom_command(
    TARGET picoG_Version
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND}
        -P "${BUILD_INC_PATH}"
)

macro(picog_add_version target)
    target_compile_definitions(${target}
        PUBLIC PICOG_VER_MAJOR=${PICOG_VER_MAJOR}
        PUBLIC PICOG_VER_MINOR=${PICOG_VER_MINOR}
        PUBLIC PICOG_VER_PATCH=${PICOG_VER_PATCH}
    )

    add_dependencies(${target} picoG_Version)
endmacro()