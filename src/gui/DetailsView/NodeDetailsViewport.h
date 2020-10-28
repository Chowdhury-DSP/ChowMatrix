#pragma once

#include "NodeDetailsComponent.h"

class NodeDetailsViewport : public Viewport
{
public:
    NodeDetailsViewport (ChowMatrix& chowMatrix);

    void resized() override;
    void paint (Graphics&) override;
    void visibleAreaChanged (const Rectangle<int>&) override;

private:
    NodeDetailsComponent detailsComp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeDetailsViewport)
};
