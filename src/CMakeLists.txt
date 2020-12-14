add_subdirectory(headless)

target_sources(ChowMatrix PRIVATE
    ChowMatrix.cpp
    NodeManager.cpp
    
    dsp/BaseNode.cpp
    dsp/DelayNode.cpp
    dsp/InputNode.cpp
    dsp/Delay/DelayProc.cpp
    dsp/Delay/VariableDelay.cpp
    dsp/Distortion/Distortion.cpp
    dsp/Distortion/LookupTables.cpp
    dsp/Parameters/BaseController.cpp
    dsp/Parameters/DelayTypeControl.cpp
    dsp/Parameters/InsanityControl.cpp
    dsp/Parameters/ParamHelpers.cpp
    dsp/Parameters/SyncControl.cpp

    gui/AutoUpdating.cpp
    gui/ParamSlider.cpp
    gui/BottomBar/BottomBarLNF.cpp
    gui/DetailsView/NodeDetails.cpp
    gui/DetailsView/NodeDetailsComponent.cpp
    gui/DetailsView/NodeDetailsGUI.cpp
    gui/DetailsView/NodeDetailsViewport.cpp
    gui/MatrixView/DelayNodeComponent.cpp
    gui/MatrixView/GraphView.cpp
    gui/MatrixView/MatrixAurora.cpp
    gui/MatrixView/NodeCompManager.cpp
    gui/MatrixView/NodeComponent.cpp

    state/StateManager.cpp
    state/presets/PresetComp.cpp
    state/presets/PresetManager.cpp
)

target_precompile_headers(ChowMatrix PRIVATE pch.h)
