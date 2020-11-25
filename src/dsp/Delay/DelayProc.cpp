#include "DelayProc.h"
#include "../Parameters/ParamHelpers.h"

void DelayProc::prepare (const dsp::ProcessSpec& spec)
{
    delay.prepare (spec);
    fs = (float) spec.sampleRate;

    feedback.reset (spec.sampleRate, 0.05 * spec.numChannels);
    inGain.reset (spec.sampleRate, 0.05 * spec.numChannels);
    delaySmooth.reset (spec.sampleRate, 0.15 * spec.numChannels);
    state.resize (spec.numChannels, 0.0f);

    reset();

    procs.prepare (spec);
}

void DelayProc::reset()
{
    delay.reset();
    procs.reset();
}

void DelayProc::flushDelay()
{
    delay.reset();
}

template<typename ProcessContext>
void DelayProc::process (const ProcessContext& context)
{
    // manage audio context
    const auto& inputBlock = context.getInputBlock();
    auto& outputBlock      = context.getOutputBlock();
    const auto numChannels = outputBlock.getNumChannels();
    const auto numSamples  = outputBlock.getNumSamples();

    jassert (inputBlock.getNumChannels() == numChannels);
    jassert (inputBlock.getNumSamples()  == numSamples);

    if (context.isBypassed)
    {
        outputBlock.copyFrom (inputBlock);
        return;
    }

    // process context
    for (size_t channel = 0; channel < numChannels; ++channel)
    {
        auto* inputSamples = inputBlock.getChannelPointer (channel);
        auto* outputSamples = outputBlock.getChannelPointer (channel);

        if (delay.isDelaySmoothing() || delaySmooth.isSmoothing())
        {
            for (size_t i = 0; i < numSamples; ++i)
            {
                delay.setDelay (delaySmooth.getNextValue());
                outputSamples[i] = processSampleSmooth (inputSamples[i], channel);
            }
        }
        else
        {
            for (size_t i = 0; i < numSamples; ++i)
                outputSamples[i] = processSample (inputSamples[i], channel);
        }
    }

    procs.get<lpfIdx>().snapToZero();
    procs.get<hpfIdx>().snapToZero();
}

template<typename SampleType>
inline SampleType DelayProc::processSample (SampleType x, size_t ch)
{
    auto input = inGain.getNextValue() * x;
    input = procs.processSample (input + state[ch]);    // process input + feedback state
    delay.pushSample ((int) ch, input);                 // push input to delay line
    auto y = delay.popSample ((int) ch);                // pop output from delay line
    state[ch] = y * feedback.getNextValue();            // save feedback state
    return y;
}

template<typename SampleType>
inline SampleType DelayProc::processSampleSmooth (SampleType x, size_t ch)
{
    auto input = inGain.getNextValue() * x;
    input = procs.processSample (input + state[ch]);    // process input + feedback state
    delay.pushSampleSmooth ((int) ch, input);           // push input to delay line
    auto y = delay.popSample ((int) ch);                // pop output from delay line
    state[ch] = y * feedback.getNextValue();            // save feedback state
    return y;
}

using IIRCoefs = chowdsp::IIR::Coefficients<float, 1>;

void DelayProc::setParameters (const Parameters& params, bool force)
{
    using namespace ParamHelpers;
    auto delayVal = (params.delayMs / 1000.0f) * fs;
    auto gainVal = params.feedback >= maxFeedback ? 0.0f : 1.0f;
    auto fbVal = params.feedback >= maxFeedback ? 1.0f
        : std::pow (jmin (params.feedback, 0.95f), 0.9f);

    if (force)
    {
        delaySmooth.setCurrentAndTargetValue (delayVal);
        inGain.setCurrentAndTargetValue (gainVal);
        feedback.setCurrentAndTargetValue (fbVal);
        delay.setDelayForce (delayVal);
    }
    else
    {
        delaySmooth.setTargetValue (delayVal);
        inGain.setTargetValue (gainVal);
        feedback.setTargetValue (fbVal);
    }

    procs.get<lpfIdx>().coefficients = IIRCoefs::makeFirstOrderLowPass ((double) fs, params.lpfFreq);
    procs.get<hpfIdx>().coefficients = IIRCoefs::makeFirstOrderHighPass ((double) fs, params.hpfFreq);
    procs.get<distortionIdx>().setGain (19.5f * std::pow (params.distortion, 2) + 0.5f);
}

//==================================================
template void DelayProc::process<dsp::ProcessContextReplacing<float>> (const dsp::ProcessContextReplacing<float>&);
template void DelayProc::process<dsp::ProcessContextNonReplacing<float>> (const dsp::ProcessContextNonReplacing<float>&);
