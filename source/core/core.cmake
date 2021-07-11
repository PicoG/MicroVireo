# source/core/vireo.cmake
# Core Vireo engine source files and configuration

set (VIREO_SOURCE_CORE
    ${VIREO_CORE_DIR}/Array.cpp
    ${VIREO_CORE_DIR}/Assert.cpp
    ${VIREO_CORE_DIR}/CEntryPoints.cpp
    ${VIREO_CORE_DIR}/CloseReference.cpp
    ${VIREO_CORE_DIR}/ControlRef.cpp
    ${VIREO_CORE_DIR}/DataQueue.cpp
    ${VIREO_CORE_DIR}/Date.cpp
    ${VIREO_CORE_DIR}/DualTypeConversion.cpp
    ${VIREO_CORE_DIR}/DualTypeEqual.cpp
    ${VIREO_CORE_DIR}/DualTypeOperation.cpp
    ${VIREO_CORE_DIR}/DualTypeVisitor.cpp
    ${VIREO_CORE_DIR}/EventLog.cpp
    ${VIREO_CORE_DIR}/Events.cpp
    ${VIREO_CORE_DIR}/ExecutionContext.cpp
    ${VIREO_CORE_DIR}/GenericFunctions.cpp
    ${VIREO_CORE_DIR}/JavaScriptDynamicRef.cpp
    ${VIREO_CORE_DIR}/JavaScriptStaticRef.cpp
    ${VIREO_CORE_DIR}/MatchPat.cpp
    ${VIREO_CORE_DIR}/Math.cpp
    ${VIREO_CORE_DIR}/NumericString.cpp
    ${VIREO_CORE_DIR}/Platform.cpp
    ${VIREO_CORE_DIR}/Queue.cpp
    ${VIREO_CORE_DIR}/RefNum.cpp
    ${VIREO_CORE_DIR}/String.cpp
    ${VIREO_CORE_DIR}/StringUtilities.cpp
    ${VIREO_CORE_DIR}/Synchronization.cpp
    ${VIREO_CORE_DIR}/TDCodecLVFlat.cpp
    ${VIREO_CORE_DIR}/TDCodecVia.cpp
    ${VIREO_CORE_DIR}/Thread.cpp
    ${VIREO_CORE_DIR}/TimeFunctions.cpp
    ${VIREO_CORE_DIR}/Timestamp.cpp
    ${VIREO_CORE_DIR}/TypeAndDataManager.cpp
    ${VIREO_CORE_DIR}/TypeAndDataReflection.cpp
    ${VIREO_CORE_DIR}/TypeDefiner.cpp
    ${VIREO_CORE_DIR}/TypeTemplates.cpp
    ${VIREO_CORE_DIR}/UnitTest.cpp
    ${VIREO_CORE_DIR}/Variants.cpp
    ${VIREO_CORE_DIR}/VirtualInstrument.cpp
    ${VIREO_CORE_DIR}/Waveform.cpp
)