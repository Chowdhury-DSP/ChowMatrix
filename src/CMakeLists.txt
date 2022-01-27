option(BUILD_HEADLESS "Build ChowMatrix headless utilities" OFF)
if(NOT (IOS OR MACOS_RELEASE) AND BUILD_HEADLESS)
    message(STATUS "Building ChowMatrix Headless!")
    add_subdirectory(headless)
endif()

target_sources(ChowMatrix PRIVATE
    ChowMatrix.cpp
    NodeManager.cpp
    
    dsp/BaseNode.cpp
    dsp/DelayNode.cpp
    dsp/InputNode.cpp
    dsp/Delay/DelayProc.cpp
    dsp/Delay/Diffusion.cpp
    dsp/Delay/Reverser.cpp
    dsp/Delay/PitchShiftWrapper.cpp
    dsp/Delay/VariableDelay.cpp
    dsp/Distortion/Distortion.cpp
    dsp/Distortion/LookupTables.cpp
    dsp/Parameters/BaseController.cpp
    dsp/Parameters/DelayTypeControl.cpp
    dsp/Parameters/HostParamControl.cpp
    dsp/Parameters/InsanityControl.cpp
    dsp/Parameters/InsanityLockHelper.cpp
    dsp/Parameters/NodeParamControl.cpp
    dsp/Parameters/RandomiseLockHelper.cpp
    dsp/Parameters/ParamHelpers.cpp
    dsp/Parameters/SyncControl.cpp

    gui/AutoUpdating.cpp
    gui/ParamSlider.cpp
    gui/BottomBar/ABComp.cpp
    gui/BottomBar/BottomBarLNF.cpp
    gui/BottomBar/HostControlMenu.cpp
    gui/BottomBar/HostControlMenuComp.cpp
    gui/BottomBar/SettingsButton.cpp
    gui/DetailsView/NodeDetails.cpp
    gui/DetailsView/NodeDetailsComponent.cpp
    gui/DetailsView/NodeDetailsGUI.cpp
    gui/DetailsView/NodeDetailsViewport.cpp
    gui/IOSUtils/LongPressActionHelper.cpp
    gui/IOSUtils/TipJar.cpp
    gui/MatrixView/DelayNodeComponent.cpp
    gui/MatrixView/GraphView.cpp
    gui/MatrixView/GraphViewport.cpp
    gui/MatrixView/MatrixAurora.cpp
    gui/MatrixView/NodeCompManager.cpp
    gui/MatrixView/NodeComponent.cpp

    state/PresetManager.cpp
    state/StateManager.cpp
)

target_precompile_headers(ChowMatrix PRIVATE pch.h)
