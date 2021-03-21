#pragma once

#include "BaseController.h"

/**
 * Utility class to manage insanity controls
 */
class InsanityControl : private BaseController,
                        private Timer
{
public:
    InsanityControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes);

    static void addParameters (Parameters& params);
    void newNodeAdded (DelayNode* newNode) override;

    void timerCallback() override;
    void parameterChanged (const String&, float newValue) override;
    std::atomic<float>* getParameter() const noexcept { return insanityParam; }

    /** 
     * Resets the delay parameters to what they were
     * before insanity was turned on.
     */
    void resetInsanityState();

private:
    void insanityStarting();
    void insanityEnding();

    std::atomic<float>* insanityParam = nullptr;
    int timerFreq = 10;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> delay_dist { -0.05f, 0.05f };
    std::uniform_real_distribution<float> pan_dist { -0.1f, 0.1f };

    float lastInsanity = 0.0f;
    std::unordered_map<Uuid, std::pair<float, float>> insanityResetMap;
    std::unordered_map<Uuid, std::pair<float, float>> insanityEndingMap;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InsanityControl)
};
