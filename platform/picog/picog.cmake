# This cmake include configures all the common settings.
# If a new target is added that doesn't work with any settings,
# those settings would need to be moved to platform specific CMakeLists

# Right now this is written for a single target build,
# I want to refactor this so that multiple targets can be built at once.
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#determine vireo source root
get_filename_component(VIREO_DIR "../../source" ABSOLUTE)

# Set the board directory and check that it exists.
if(NOT PICOG_BOARD_DIR)
    set(PICOG_BOARD_DIR ${CMAKE_SOURCE_DIR}/boards/${PICOG_BOARD})
endif()
if(NOT EXISTS ${PICOG_BOARD_DIR}/picog_board.cmake)
    message(FATAL_ERROR "Invalid PICOG_BOARD specified: ${PICOG_BOARD}")
endif()

# Include board config which brings in board specific pinouts and peripherals
include(${PICOG_BOARD_DIR}/picog_board.cmake)

# Include the configuration for vireo and the main vireo source
include(${CMAKE_CURRENT_LIST_DIR}/vireo_config.cmake)
include(${VIREO_DIR}/vireo.cmake)

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
function(create_picog_build target output)

    set(PICOG_TARGET "picoG_${PICOG_PLATFORM}_${PICOG_BOARD}")
    set(PICOG_OUTPUT "${PICOG_TARGET}_${PICOG_VERSION}")

    #Forward target name out to caller in specified variable name
    #${target} contains the name of the parent variable to store the value in
    set(${target} "${PICOG_TARGET}" PARENT_SCOPE)
    set(${output} "${PICOG_OUTPUT}" PARENT_SCOPE)

    #Begin the target build configuration
    add_executable(${PICOG_TARGET})
    set_target_properties(${PICOG_TARGET} PROPERTIES OUTPUT_NAME "${PICOG_OUTPUT}")

    #configure the define symbols that get passed into the source based on the build configuration
    target_compile_definitions(${PICOG_TARGET}
        PUBLIC _PICOG_PLATFORM=${PICOG_PLATFORM}
        PUBLIC    _PICOG_BOARD=${PICOG_BOARD}
        PUBLIC PICOG_VER_MAJOR=${PICOG_VER_MAJOR}
        PUBLIC PICOG_VER_MINOR=${PICOG_VER_MINOR}
        PUBLIC PICOG_VER_PATCH=${PICOG_VER_PATCH}
    )

    #Add the Vireo engine sources to the build
    target_sources(${PICOG_TARGET} PUBLIC
        ${VIREO_SOURCE_CORE}
        ${VIREO_SOURCE_IO}
    )

    #Specify the vireo and picoG include paths for header includes
    target_include_directories(${PICOG_TARGET}
        PUBLIC ${VIREO_INCLUDE}
        PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../picog/include
    )

    #add a dep on the picoG_Version pseudo target described above to get it to update whenever a build is performed
    add_dependencies(${PICOG_TARGET} picoG_Version)
endfunction()