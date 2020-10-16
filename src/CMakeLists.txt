target_sources(ChowMatrix PRIVATE
    ChowMatrix.cpp
    
    dsp/BaseNode.cpp
    dsp/DelayNode.cpp
    dsp/InputNode.cpp

    gui/ParamSlider.cpp
    gui/DetailsView/NodeDetailsComponent.cpp
    gui/MatrixView/DelayNodeComponent.cpp
    gui/MatrixView/GraphView.cpp
    gui/MatrixView/NodeCompManager.cpp
    gui/MatrixView/NodeComponent.cpp
)
    