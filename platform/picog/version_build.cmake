macro(picog_version_build trg)
    add_custom_command(
        TARGET ${trg}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND}
            -P "build_increment.cmake"
    )
endmacro()