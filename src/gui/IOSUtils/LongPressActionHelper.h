#pragma once

#include <pch.h>

#if JUCE_IOS

class LongPressActionHelper : private Timer
{
public:
    LongPressActionHelper (float dragDistanceThreshold = 8.0f, int pressLengthMilliseconds = 800);
    virtual ~LongPressActionHelper();

    std::function<void (Point<int> downPosition)> longPressCallback = [] (Point<int>) {};

    void startPress (const Point<int>& newDownPosition);
    void abortPress();

    bool isBeingPressed() const noexcept
    {
        return pressStarted;
    }

    void setDragDistance (float newDistance)
    {
        dragDistance = newDistance;
    }

    bool isTouchEnabled() const
    {
        return touchEnabled;
    }

    void setTouchEnabled (bool shouldBeEnabled)
    {
        touchEnabled = shouldBeEnabled;
    }

private:
    void timerCallback() override;

    Point<int> downPosition;
    bool pressStarted = false;
    float dragDistance = 8.0f;

    const float dragDistanceThreshold;
    const int pressLengthMs;

    bool touchEnabled = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LongPressActionHelper)
};

#endif // JUCE_IOS
