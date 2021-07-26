# source/vireo.cmake
# top-level vireo engine source definition and main feature configuration

set(VIREO_CORE_DIR "${VIREO_DIR}/core")
set(VIREO_IO_DIR "${VIREO_DIR}/io")

list(APPEND VIREO_INCLUDE "${VIREO_DIR}/include")

include(${VIREO_CORE_DIR}/core.cmake)
include(${VIREO_IO_DIR}/io.cmake)