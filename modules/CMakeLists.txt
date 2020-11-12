add_subdirectory(JUCE)
juce_add_modules(foleys_gui_magic)
juce_add_modules(chowdsp_utils)
add_subdirectory(polylogarithm)

add_library(juce_plugin_modules STATIC)

target_link_libraries(juce_plugin_modules
    PRIVATE
        juce::juce_audio_utils
        foleys_gui_magic
        chowdsp_utils
        polylogarithm
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

target_compile_definitions(juce_plugin_modules
    PUBLIC
        JUCE_REPORT_APP_USAGE=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JucePlugin_Manufacturer="chowdsp"
        JucePlugin_VersionString="${CMAKE_PROJECT_VERSION}"
        JucePlugin_Name="${CMAKE_PROJECT_NAME}"
    INTERFACE
        $<TARGET_PROPERTY:juce_plugin_modules,COMPILE_DEFINITIONS>
)

if(MSVC)
    target_compile_options(juce_plugin_modules PRIVATE
        /wd4458
    )
endif()

target_include_directories(juce_plugin_modules
    INTERFACE
        $<TARGET_PROPERTY:juce_plugin_modules,INCLUDE_DIRECTORIES>
)

set_target_properties(juce_plugin_modules PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE
    VISIBILITY_INLINES_HIDDEN TRUE
    C_VISBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
)
