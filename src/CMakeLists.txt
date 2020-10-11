target_sources(ChowMatrix PRIVATE
    ChowMatrix.cpp
    
    dsp/BaseNode.cpp
    dsp/DelayNode.cpp
    dsp/InputNode.cpp

    gui/DelayNodeComponent.cpp
    gui/GraphView.cpp
    gui/NodeCompManager.cpp
    gui/NodeComponent.cpp
    gui/ParamSlider.cpp
)
    