cmake_minimum_required(VERSION 3.15)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment target")
project(ChowMatrix VERSION 1.3.0)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(modules)
include_directories(modules)

# juce_set_vst2_sdk_path(C:/SDKs/VST_SDK/VST2_SDK/)
# juce_set_aax_sdk_path(NONE)

# set default plugin formats to build
if(IOS)
    set(JUCE_FORMATS Standalone AUv3)
else()
    set(JUCE_FORMATS AU VST3 Standalone)
endif()

# Build LV2 only on Linux
if(UNIX AND NOT APPLE)
    message(STATUS "Building LV2 plugin format")
    list(APPEND JUCE_FORMATS LV2)
endif()

# Build VST2 if SDK target exists
if(TARGET juce_vst2_sdk)
    message(STATUS "Building VST2 plugin format")
    list(APPEND JUCE_FORMATS VST)
endif()

# Build AAX if SDK target exists
if(TARGET juce_aax_sdk)
    message(STATUS "Building AAX plugin format")
    list(APPEND JUCE_FORMATS AAX)
endif()

juce_add_plugin(ChowMatrix
    COMPANY_NAME chowdsp
    PLUGIN_MANUFACTURER_CODE Chow
    PLUGIN_CODE spg3
    FORMATS ${JUCE_FORMATS}
    ProductName "ChowMatrix"

    VST2_CATEGORY kPlugCategEffect
    VST3_CATEGORIES Fx Delay
    AU_MAIN_TYPE kAudioUnitType_Effect
    AAX_CATEGORY AAX_ePlugInCategory_Delay

    LV2_URI https://github.com/Chowdhury-DSP/ChowMatrix

    ICON_BIG res/logo.png
    MICROPHONE_PERMISSION_ENABLED TRUE
    NEEDS_STORE_KIT TRUE
    REQUIRES_FULL_SCREEN TRUE
    IPHONE_SCREEN_ORIENTATIONS UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight
    IPAD_SCREEN_ORIENTATIONS UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight
)

# create JUCE header
juce_generate_juce_header(ChowMatrix)

# add sources
add_subdirectory(src)
include_directories(src)
add_subdirectory(res)

target_compile_definitions(ChowMatrix
  PUBLIC
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    CHOWDSP_AUTO_UPDATE=1
)

if(ASIOSDK_DIR)
    message(STATUS "Using ASIO SDK from ${ASIOSDK_DIR}")
    target_include_directories(juce_plugin_modules PUBLIC ${ASIOSDK_DIR}/common)
    target_compile_definitions(juce_plugin_modules PUBLIC JUCE_ASIO=1)
endif()

target_link_libraries(ChowMatrix PUBLIC
    juce_plugin_modules
)

option(RUN_PLUGINVAL "Run pluginval on ChowMatrix plugins" OFF)
if(RUN_PLUGINVAL)
    create_pluginval_target(ChowMatrix_VST3 "ChowMatrix.vst3")
endif()

# we need these flags for notarization on MacOS
option(MACOS_RELEASE "Set build flags for MacOS Release" OFF)
if(MACOS_RELEASE)
    message(STATUS "Setting MacOS release flags...")
    set_target_properties(ChowMatrix_Standalone PROPERTIES
        XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)
endif()

if(IOS)
    message(STATUS "Setting iOS-specific properties...")

    foreach(target IN ITEMS BinaryData polylogarithm juce_plugin_modules ChowMatrix ChowMatrix_Standalone ChowMatrix_AUv3)
        set_target_properties(${target}
            PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "./"
                ARCHIVE_OUTPUT_DIRECTORY "./"
                LIBRARY_OUTPUT_DIRECTORY "./")
    endforeach()

    set_target_properties(ChowMatrix_Standalone PROPERTIES
            XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)"
            XCODE_ATTRIBUTE_SKIP_INSTALL "NO"
            XCODE_ATTRIBUTE_ENABLE_IN_APP_PURCHASE "YES")

    set_target_properties(ChowMatrix_AUv3 PROPERTIES
            XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)/ChowMatrix.app/PlugIns"
            XCODE_ATTRIBUTE_SKIP_INSTALL "NO"
            XCODE_ATTRIBUTE_ENABLE_IN_APP_PURCHASE "YES")
endif()
