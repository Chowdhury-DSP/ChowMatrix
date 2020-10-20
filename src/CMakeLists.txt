target_sources(ChowMatrix PRIVATE
    ChowMatrix.cpp
    
    dsp/BaseNode.cpp
    dsp/DelayNode.cpp
    dsp/DelayProc.cpp
    dsp/Distortion.cpp
    dsp/InputNode.cpp
    dsp/LookupTables.cpp
    dsp/ParamHelpers.cpp

    gui/ParamSlider.cpp
    gui/BottomBar/BottomBarLNF.cpp
    gui/DetailsView/NodeDetailsComponent.cpp
    gui/MatrixView/DelayNodeComponent.cpp
    gui/MatrixView/GraphView.cpp
    gui/MatrixView/NodeCompManager.cpp
    gui/MatrixView/NodeComponent.cpp
)
    