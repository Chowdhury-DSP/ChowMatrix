#include "Reverser.h"

namespace
{
int nearestEvenInt (int to)
{
  return (to % 2 == 0) ? to : (to + 1);
}
} // namespace

void Reverser::prepare (const dsp::ProcessSpec& spec)
{
    fs = (float) spec.sampleRate;
    maxWindowSize = (int) fs; // 1 second

    windowBuffer.setSize (1, maxWindowSize);
    window = windowBuffer.getWritePointer (0);

    reverseBuffer.setSize (1, maxWindowSize * 4);
    revBuffPtr = reverseBuffer.getWritePointer (0);

    reset();
}

void Reverser::reset()
{  
    reverseBuffer.clear();
    windowSize = -1; // this will get reset next time setReverseTime is called
}

void Reverser::setReverseTime (float revTimeMs)
{
    auto newWindowSize = nearestEvenInt (int (revTimeMs * fs / 1000.0f));
    if (windowSize == newWindowSize)
        return;

    if (newWindowSize == 0)
    {
        windowSize = 0;
        reverseBuffer.clear();
        bypass = true;
        return;
    }

    bypass = false;

    windowSize = newWindowSize;
    doubleWindowSize = 2 * windowSize;
    halfWindowSize = windowSize / 2;

    bufferWrite = 0;
    bufferRead1 = doubleWindowSize;
    bufferRead2 = bufferRead1 + halfWindowSize;

    dsp::WindowingFunction<float>::fillWindowingTables (window,
                                                        (size_t) windowSize,
                                                        dsp::WindowingFunction<float>::hann);
}
