#pragma once

#include "NodeDetailsComponent.h"

class NodeDetailsViewport : public Viewport
{
public:
    NodeDetailsViewport (ChowMatrix& chowMatrix) :
        detailsComp (chowMatrix)
    {
        setViewedComponent (&detailsComp, false);
        setScrollBarsShown (false, true);
    }

    void resized() override
    {
        detailsComp.setSize (detailsComp.getWidth(), getHeight());
        detailsComp.setMinWidth (getWidth());
    }

private:
    NodeDetailsComponent detailsComp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeDetailsViewport)
};
