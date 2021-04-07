#include "../../ChowMatrix.h"

class PerformanceTest : public UnitTest
{
public:
    using DelayType = VariableDelay::DelayType;

    PerformanceTest() : UnitTest ("Performance Test")
    {
    }

    void runTest() override
    {
        constexpr double rtLimit = 250.0;

        beginTest ("No Interp Test");
        runPerfTest (DelayType::NoInterp, rtLimit);
        
        beginTest ("Linear Interp Test");
        runPerfTest (DelayType::LinearInterp, rtLimit);

        beginTest ("Lagrange 3rd Interp Test");
        runPerfTest (DelayType::ThirdInterp, rtLimit);

        beginTest ("Lagrange 5th Interp Test");
        runPerfTest (DelayType::FifthInterp, rtLimit);

        beginTest ("Small Sinc Interp Test");
        runPerfTest (DelayType::Sinc16, rtLimit);

        beginTest ("Large Sinc Interp Test");
        runPerfTest (DelayType::Sinc32, rtLimit);

        beginTest ("Small BBD Interp Test");
        runPerfTest (DelayType::BBDShort, rtLimit);

        beginTest ("Large BBD Interp Test");
        runPerfTest (DelayType::BBDLong, rtLimit);
    }

    void setDelayMode (AudioProcessor* plugin, DelayType mode)
    {
        auto params = plugin->getParameters();
        for (auto param : params)
        {
            if (param->getName (1024) == "Delay Type")
            {
                param->setValue ((float) mode / 7.0f);
                std::cout << "Setting parameter " << param->getName (1024)
                          << ": " << param->getText (param->getValue(), 1024) << std::endl;
            }
        }
    }

    void runPerfTest (DelayType mode, double rtLimit)
    {
        std::unique_ptr<AudioProcessor> plugin (createPluginFilterOfType (AudioProcessor::WrapperType::wrapperType_Standalone));

        constexpr double sampleRate = 48000.0;
        constexpr int nSamples = 1024;
        constexpr int nIters = 500;

        AudioBuffer<float> buffer (2, nSamples);
        MidiBuffer mb;
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* x = buffer.getWritePointer (ch);
            for (int n = 0; n < nSamples; ++n)
                x[n] = getRandom().nextFloat() * 2.0f - 1.0f;
        }

        plugin->prepareToPlay (sampleRate, nSamples);
        plugin->setCurrentProgram (1);
        Thread::sleep (250);
        setDelayMode (plugin.get(), mode);

        for (int i = 0; i < 10; ++i)
            plugin->processBlock (buffer, mb);

        Time time;
        auto start = time.getMillisecondCounterHiRes();
        for (int i = 0; i < nIters; ++i)
            plugin->processBlock (buffer, mb);

        auto duration = (time.getMillisecondCounterHiRes() - start) / 1000.0;
        plugin->releaseResources();

        auto audioSeconds = nSamples * nIters / sampleRate;
        auto rtFactor = audioSeconds / duration;
        std::cout << "Processed " << audioSeconds << " seconds of audio in " << duration << " seconds" << std::endl;
        std::cout << "Realtime Factor: " << rtFactor << "x" << std::endl;

        expectGreaterThan (rtFactor, rtLimit, "Delay Mode " + String (mode) + " is too slow!");
    }
};

static PerformanceTest perfTest;
