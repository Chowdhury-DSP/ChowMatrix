#include "LongPressActionHelper.h"

#if JUCE_IOS

LongPressActionHelper::LongPressActionHelper (float dragDistanceThresh, int pressLengthMilliseconds) : dragDistanceThreshold (dragDistanceThresh),
                                                                                                       pressLengthMs (pressLengthMilliseconds)
{
}

LongPressActionHelper::~LongPressActionHelper()
{
    stopTimer();
}

void LongPressActionHelper::startPress (const Point<int>& newDownPosition)
{
    if (! touchEnabled)
        return;

    pressStarted = true;
    downPosition = newDownPosition;
    startTimer (pressLengthMs);
}

void LongPressActionHelper::abortPress()
{
    stopTimer();
    pressStarted = false;
    dragDistance = 0.0f;
}

void LongPressActionHelper::timerCallback()
{
    bool wasPressStarted = pressStarted;
    bool wasDragged = dragDistance > dragDistanceThreshold;
    abortPress();

    if (! wasPressStarted)
    {
        // No press started? That's not good.
        jassertfalse;
        return;
    }

    if (wasDragged)
        return;

    longPressCallback (downPosition);
}

#endif // JUCE_IOS
