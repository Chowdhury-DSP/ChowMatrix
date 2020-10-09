#pragma once

#include <JuceHeader.h>

struct NodeListener
{
public:
    virtual ~NodeListener() {}

    virtual void nodeCreated() {}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeListener)
};
