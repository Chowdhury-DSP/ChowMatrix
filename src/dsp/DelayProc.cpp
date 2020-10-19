#include "DelayProc.h"

void DelayProc::prepare (const dsp::ProcessSpec& spec)
{
    delay.prepare (spec);
    fs = (float) spec.sampleRate;

    feedback.reset (spec.sampleRate, 0.05 * spec.numChannels);
    state.resize (spec.numChannels, 0.0f);

    reset();

    procs.prepare (spec);
}

void DelayProc::reset()
{
    delay.reset();
    procs.reset();
}

template<typename ProcessContext>
void DelayProc::process (const ProcessContext& context)
{
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

    for (size_t channel = 0; channel < numChannels; ++channel)
    {
        auto* inputSamples = inputBlock.getChannelPointer (channel);
        auto* outputSamples = outputBlock.getChannelPointer (channel);

        for (size_t i = 0; i < numSamples; ++i)
        {
            auto input = procs.processSample (inputSamples[i] + state[channel]);
            delay.pushSample ((int) channel, input);
            outputSamples[i] = delay.popSample ((int) channel);
            state[channel] = outputSamples[i] * feedback.getNextValue();
        }
    }
}

using IIRCoefs = dsp::IIR::Coefficients<float>;

void DelayProc::setParameters (const Parameters& params)
{
    delay.setDelay ((params.delayMs / 1000.0f) * fs);
    feedback.setTargetValue (params.feedback);
    procs.get<lpfIdx>().coefficients = IIRCoefs::makeFirstOrderLowPass ((double) fs, params.lpfFreq);
    procs.get<hpfIdx>().coefficients = IIRCoefs::makeFirstOrderHighPass ((double) fs, params.hpfFreq);
    procs.get<distortionIdx>().setGain (19.8f * std::pow (params.distortion, 2) + 0.2f);
}

//==================================================
template void DelayProc::process<dsp::ProcessContextReplacing<float>> (const dsp::ProcessContextReplacing<float>&);
template void DelayProc::process<dsp::ProcessContextReplacing<double>> (const dsp::ProcessContextReplacing<double>&);

template void DelayProc::process<dsp::ProcessContextNonReplacing<float>> (const dsp::ProcessContextNonReplacing<float>&);
template void DelayProc::process<dsp::ProcessContextNonReplacing<double>> (const dsp::ProcessContextNonReplacing<double>&);
