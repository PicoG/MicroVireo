include(${CMAKE_CURRENT_LIST_DIR}/version.cmake)

set(BUILD_HEADER "${CMAKE_CURRENT_LIST_DIR}/include/picog/build.h")
set(BUILD_CACHE "${CMAKE_CURRENT_LIST_DIR}/include/picog/version_build.txt")

get_filename_component(BUILD_INC_PATH "${CMAKE_CURRENT_LIST_DIR}/build_timestamp.cmake" ABSOLUTE)

set(PICOG_VERSION "${PICOG_VER_MAJOR}.${PICOG_VER_MINOR}.${PICOG_VER_PATCH}")

#This custom target is used to create a target that creates the build timestamp
#This needs to be a separate target because we can then force the actual firmware
#to depend on it which will cause it to be built first.
add_custom_target(picoG_Version)
add_custom_command(
    TARGET picoG_Version
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND}
        -P "${BUILD_INC_PATH}"
)

#set_picog_build is the main macro to configure a target as picoG firmware
macro(set_picog_build target)
    target_compile_definitions(${target}
        PUBLIC _PICOG_PLATFORM=${VIREO_PLATFORM}
        PUBLIC _PICOG_BOARD=${VIREO_BOARD}
        PUBLIC PICOG_VER_MAJOR=${PICOG_VER_MAJOR}
        PUBLIC PICOG_VER_MINOR=${PICOG_VER_MINOR}
        PUBLIC PICOG_VER_PATCH=${PICOG_VER_PATCH}
    )

    add_dependencies(${target} picoG_Version)
endmacro()