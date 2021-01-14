#pragma once

#include "../Parameters/ParamHelpers.h"

/**
 * Utils for converting delay parameters
 * to tempo-synced delay times.
 */
namespace TempoSyncUtils
{
/** A simple struct containing a rhythmic delay length */
struct DelayRhythm
{
    constexpr DelayRhythm (const std::string_view& name, const std::string_view& label, double tempoFactor) : name (name),
                                                                                                              label (label),
                                                                                                              tempoFactor (tempoFactor) {}

    inline String getLabel() const { return String (static_cast<std::string> (label)); }

    std::string_view name;
    std::string_view label;
    double tempoFactor;
};

static constexpr std::array<DelayRhythm, 19> rhythms {
    DelayRhythm ("Thirty-Second", "1/32", 0.125),
    DelayRhythm ("Thirty-Second Dot", "1/32 D", 0.125 * 1.5),
    DelayRhythm ("Sixteenth Triplet", "1/16 T", 0.5 / 3.0),
    DelayRhythm ("Sixteenth", "1/16", 0.25),
    DelayRhythm ("Sixteenth Dot", "1/16 D", 0.25 * 1.5),
    DelayRhythm ("Eigth Triplet", "1/8 T", 1.0 / 3.0),
    DelayRhythm ("Eigth", "1/8", 0.5),
    DelayRhythm ("Eigth Dot", "1/8 D", 0.5 * 1.5),
    DelayRhythm ("Quarter Triplet", "1/4 T", 2.0 / 3.0),
    DelayRhythm ("Quarter", "1/4", 1.0),
    DelayRhythm ("Quarter Dot", "1/4 D", 1.0 * 1.5),
    DelayRhythm ("Half Triplet", "1/2 T", 4.0 / 3.0),
    DelayRhythm ("Half", "1/2", 2.0),
    DelayRhythm ("Half Dot", "1/2 D", 2.0 * 1.5),
    DelayRhythm ("Whole Triplet", "1/1 T", 8.0 / 3.0),
    DelayRhythm ("Whole", "1/1", 4.0),
    DelayRhythm ("Whole Dot", "1/1 D", 4.0 * 1.5),
    DelayRhythm ("Two Whole Triplet", "2/1 T", 16.0 / 3.0),
    DelayRhythm ("Two Whole", "2/1", 8.0),
};

/** Return time in seconds for rhythm and tempo */
static inline double getTimeForRythm (double tempoBPM, const DelayRhythm& rhythm)
{
    const auto beatLength = 1.0 / (tempoBPM / 60.0);
    return beatLength * rhythm.tempoFactor;
}

/** Returns the corresponding rhythm for a 0-1 param value */
static inline const DelayRhythm& getRhythmForParam (float param01)
{
    auto idx = static_cast<size_t> ((rhythms.size() - 1) * std::pow (param01, 1.5f));
    return rhythms[idx];
}

/** Sine-wave oscillator synced to the song tempo */
class SyncedLFO : public chowdsp::SineWave<float>
{
public:
    SyncedLFO() {}
    virtual ~SyncedLFO() {}

    // Sets the correct oscillator frequency for a given tempo
    void setFreqSynced (const Parameter* freqParam, float tempoBPM)
    {
        const auto& rhythm = getRhythmForParam (freqParam->convertTo0to1 (*freqParam));
        float freqValue = 1.0f / (float) getTimeForRythm ((double) tempoBPM, rhythm);
        setFrequency (freqValue);
    }

    // resets the LFO when the DAW starts playing
    void setPlayHead (AudioPlayHead* playhead)
    {
        if (playhead == nullptr)
            return;

        AudioPlayHead::CurrentPositionInfo info;
        playhead->getCurrentPosition (info);

        if (info.isPlaying && ! wasPlaying)
            reset();

        wasPlaying = info.isPlaying;
    }

private:
    bool wasPlaying = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SyncedLFO)
};

} // namespace TempoSyncUtils
