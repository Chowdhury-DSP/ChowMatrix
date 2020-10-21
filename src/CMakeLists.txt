target_sources(ChowMatrix PRIVATE
    ChowMatrix.cpp
    NodeManager.cpp
    
    dsp/BaseNode.cpp
    dsp/DelayNode.cpp
    dsp/DelayProc.cpp
    dsp/Distortion.cpp
    dsp/InputNode.cpp
    dsp/LookupTables.cpp
    dsp/ParamHelpers.cpp

    gui/ParamSlider.cpp
    gui/BottomBar/BottomBarLNF.cpp
    gui/DetailsView/NodeDetails.cpp
    gui/DetailsView/NodeDetailsComponent.cpp
    gui/DetailsView/NodeDetailsGUI.cpp
    gui/DetailsView/NodeDetailsViewport.cpp
    gui/MatrixView/DelayNodeComponent.cpp
    gui/MatrixView/GraphView.cpp
    gui/MatrixView/NodeCompManager.cpp
    gui/MatrixView/NodeComponent.cpp
)
    