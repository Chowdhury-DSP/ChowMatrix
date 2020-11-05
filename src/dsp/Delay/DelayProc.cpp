#include "DelayProc.h"

void DelayProc::prepare (const dsp::ProcessSpec& spec)
{
    delay.prepare (spec);
    fs = (float) spec.sampleRate;

    feedback.reset (spec.sampleRate, 0.05 * spec.numChannels);
    delaySmooth.reset (spec.sampleRate, 0.05 * spec.numChannels);
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

        for (size_t i = 0; i < numSamples; ++i)
        {
            delay.setDelay (delaySmooth.getNextValue());
            auto input = procs.processSample (inputSamples[i] + state[channel]);    // process input + feedback state
            delay.pushSample ((int) channel, input);                                // push input to delay line
            outputSamples[i] = delay.popSample ((int) channel);                     // pop output from delay line
            state[channel] = outputSamples[i] * feedback.getNextValue();            // save feedback state
        }
    }
}

using IIRCoefs = dsp::IIR::Coefficients<float>;

void DelayProc::setParameters (const Parameters& params)
{
    delaySmooth.setTargetValue ((params.delayMs / 1000.0f) * fs);
    feedback.setTargetValue (params.feedback);
    procs.get<lpfIdx>().coefficients = IIRCoefs::makeFirstOrderLowPass ((double) fs, params.lpfFreq);
    procs.get<hpfIdx>().coefficients = IIRCoefs::makeFirstOrderHighPass ((double) fs, params.hpfFreq);
    procs.get<distortionIdx>().setGain (19.5f * std::pow (params.distortion, 2) + 0.5f);
}

//==================================================
template void DelayProc::process<dsp::ProcessContextReplacing<float>> (const dsp::ProcessContextReplacing<float>&);
template void DelayProc::process<dsp::ProcessContextReplacing<double>> (const dsp::ProcessContextReplacing<double>&);

template void DelayProc::process<dsp::ProcessContextNonReplacing<float>> (const dsp::ProcessContextNonReplacing<float>&);
template void DelayProc::process<dsp::ProcessContextNonReplacing<double>> (const dsp::ProcessContextNonReplacing<double>&);