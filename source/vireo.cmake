# source/vireo.cmake
# top-level vireo engine source definition and main feature configuration

set(VIREO_CORE_DIR "${VIREO_DIR}/core")

list(APPEND VIREO_INCLUDE "${VIREO_DIR}/include")

include(${VIREO_CORE_DIR}/core.cmake)