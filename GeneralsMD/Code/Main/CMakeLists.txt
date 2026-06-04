if(APPLE)
    add_executable(generalsmd_macos_main
        MacMain.cpp
    )
    target_link_libraries(generalsmd_macos_main PRIVATE generalsmd_platform)
    if(CNC_HAS_SDL3)
        target_compile_definitions(generalsmd_macos_main PRIVATE CNC_HAS_SDL3=1)
        target_link_libraries(generalsmd_macos_main PRIVATE ${CNC_SDL3_TARGET})
    endif()
endif()
