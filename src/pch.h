#pragma once

/**
 * Precompiled headers for ChowMatrix
 */

// C++ STL
#include <deque>
#include <future>
#include <random>
#include <unordered_map>

// JUCE modules
#include <JuceHeader.h>

// Things I don't plan on touching
#include "dsp/Delay/DelayStore.h"
#include "dsp/Distortion/LookupTables.h"
#include "dsp/ProcessorBase.h"
#include "dsp/ProcessorChain.h"
