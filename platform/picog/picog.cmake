

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set build type to reduce firmware size
# Used by pico-sdk
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE MinSizeRel)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#See if PICK_SDK_PATH already defined by environment, otherwise set it to default
if (DEFINED ENV{PICO_SDK_PATH})
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
else ()
    #default to looking for pico-sdk as a sibling of the Vireo folder
    get_filename_component(PICO_SDK_PATH "../../../pico-sdk" ABSOLUTE)
    if (NOT EXISTS ${PICO_SDK_PATH}/pico_sdk_init.cmake)
        message(FATAL_ERROR "PICO_SDK_PATH variable not set in environment.")
    endif()
endif()

#determine vireo source root
get_filename_component(VIREO_DIR "../../source" ABSOLUTE)

# Set the board directory and check that it exists.
if(NOT VIREO_BOARD_DIR)
    set(VIREO_BOARD_DIR ${CMAKE_SOURCE_DIR}/boards/${VIREO_BOARD})
endif()
if(NOT EXISTS ${VIREO_BOARD_DIR}/vireoboard.cmake)
    message(FATAL_ERROR "Invalid VIREO_BOARD specified: ${VIREO_BOARD}")
endif()

# Include board config which brings in board specific pinouts and peripherals
include(${VIREO_BOARD_DIR}/vireoboard.cmake)

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
function(create_picog_build target)

    set(PICOG_TARGET "picoG_${VIREO_PLATFORM}_${VIREO_BOARD}_${PICOG_VERSION}")

    #Forward target name out to caller in specified variable name
    #${target} contains the name of the parent variable to store the value in
    set(${target} "${PICOG_TARGET}" PARENT_SCOPE)

    #Begin the target build configuration
    add_executable(${PICOG_TARGET})

    #configure the define symbols that get passed into the source based on the build configuration
    target_compile_definitions(${PICOG_TARGET}
        PUBLIC _PICOG_PLATFORM=${VIREO_PLATFORM}
        PUBLIC    _PICOG_BOARD=${VIREO_BOARD}
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