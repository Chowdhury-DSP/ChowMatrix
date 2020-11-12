#pragma once

#include <pch.h>

/**
 * Utils for converting delay parameters
 * to tempo-synced delay times.
 */ 
namespace DelaySyncUtils
{

/** A simple struct containing a rhythmic delay length */
struct DelayRhythm
{
    constexpr DelayRhythm (const String& name, const String& label, double tempoFactor) :
        name (name),
        label (label),
        tempoFactor (tempoFactor) {}

    const String name;
    const String label;
    const double tempoFactor;
};

const static std::array<DelayRhythm, 12> rhythms {
    DelayRhythm ("Sixteenth Triplet", "1/16 T", 0.5 / 3.0 ),
    DelayRhythm ("Sixteenth",         "1/16",   0.25      ),
    DelayRhythm ("Sixteenth Dot",     "1/16 D", 0.25 * 1.5),
    DelayRhythm ("Eigth Triplet",     "1/8 T",  1.0 / 3.0 ),
    DelayRhythm ("Eigth",             "1/8",    0.5       ),
    DelayRhythm ("Eigth Dot",         "1/8 D",  0.5 * 1.5 ),
    DelayRhythm ("Quarter Triplet",   "1/4 T",  2.0 / 3.0 ),
    DelayRhythm ("Quarter",           "1/4",    1.0       ),
    DelayRhythm ("Quarter Dot",       "1/4 D",  1.0 * 1.5 ),
    DelayRhythm ("Half Triplet",      "1/2 T",  4.0 / 3.0 ),
    DelayRhythm ("Half",              "1/2",    2.0       ),
    DelayRhythm ("Half Dot",          "1/2 D",  2.0 * 1.5 ),
};

/** Return delay in seconds for rhythm and tempo */
static inline double getDelayForRythm (double tempoBPM, const DelayRhythm& rhythm)
{
    const auto beatLength = 1.0 / (tempoBPM / 60.0);
    return beatLength * rhythm.tempoFactor;
}

static inline const DelayRhythm& getRhythmForParam (float param01)
{
    auto idx = static_cast<size_t> ((rhythms.size() - 1) * param01);
    return rhythms[idx];
}

}
