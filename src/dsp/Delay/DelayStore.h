#pragma once

#include "VariableDelay.h"

/** A store to create delay objects more quickly. */
class DelayStore
{
public:
    DelayStore()
    {
        // start with a bunch in the queue
        for (int i = 0; i < storeSize; ++i)
            loadNewDelay();
    }

    VariableDelay* getNextDelay()
    {
        SpinLock::ScopedLockType nextDelayLock (delayStoreLock);

        auto* delay = delayFutureStore.front().get().release();
        delayFutureStore.pop_front();
        loadNewDelay();

        return delay;
    }

private:
    void loadNewDelay()
    {
        delayFutureStore.push_back (std::async (std::launch::async, [] {
            auto newDelay = std::make_unique<VariableDelay> (1 << 19);
            newDelay->prepare ({ 48000.0, 512, 1 });
            newDelay->reset();
            return std::move (newDelay);
        }));
    }

    static constexpr int storeSize = 8;

    std::deque<std::future<std::unique_ptr<VariableDelay>>> delayFutureStore;
    SpinLock delayStoreLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayStore)
};
