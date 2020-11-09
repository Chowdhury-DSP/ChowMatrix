#pragma once

#include <pch.h>
#include "../InputNode.h"

/**
 * Utility class to manage insanity controls
 */ 
class InsanityControl : private AudioProcessorValueTreeState::Listener,
                        private DBaseNode::Listener,
                        private Timer
{
public:
    using Parameters = std::vector<std::unique_ptr<juce::RangedAudioParameter>>;

    InsanityControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes);
    ~InsanityControl();

    static void addParameters (Parameters& params);
    void nodeAdded (DelayNode* newNode) override;
    void nodeRemoved (DelayNode* nodeToRemove) override;

    void timerCallback() override;
    void parameterChanged (const String&, float newValue) override;
    std::atomic<float>* getParameter() const noexcept { return insanityParam; }

private:
    AudioProcessorValueTreeState& vts;
    std::array<InputNode, 2>* nodes;

    std::atomic<float>* insanityParam = nullptr;
    int timerFreq = 1;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> delay_dist { -0.05f, 0.05f };
    std::uniform_real_distribution<float> pan_dist { -0.1f, 0.1f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InsanityControl)
};
