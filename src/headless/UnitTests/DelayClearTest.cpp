#include "../../ChowMatrix.h"

class DelayClearTest : public UnitTest
{
public:
    DelayClearTest() : UnitTest ("Delay Clear Test")
    {
    }

    void runTest() override
    {
        beginTest ("Delay Clear Test");
        auto proc = createPluginFilterOfType (AudioProcessor::WrapperType::wrapperType_Standalone);
        std::unique_ptr<ChowMatrix> plugin (dynamic_cast<ChowMatrix*> (proc));

        NodeManager::doForNodes (plugin->getNodes(), [] (DelayNode* d) {
            d->setNodeParameter ("FDBK", 0.9f);
            d->setNodeParameter ("DLY", 0.1f);
        });

        constexpr double sampleRate = 48000.0;
        constexpr int nSamples = 1024;
        plugin->prepareToPlay (sampleRate, nSamples);

        MessageManager::getInstance()->runDispatchLoopUntil (50);

        // inject signal into delays
        AudioBuffer<float> buffer (2, nSamples);
        buffer.clear();
        for (int ch = 0; ch < 2; ++ch)
            buffer.setSample (ch, 100, 1.0f);
        plugin->processAudioBlock (buffer);
        
        {
            auto mag = buffer.getMagnitude (0, nSamples);
            std::cout << "Starting Magnitude: " << mag << std::endl;
        }

        NodeManager::doForNodes (plugin->getNodes(), [] (DelayNode* n) { n->flushDelays(); });

        // look for echoes...
        for (int i = 0; i < 5; ++i)
        {
            buffer.clear();
            plugin->processAudioBlock (buffer);
            auto mag = buffer.getMagnitude (0, nSamples);
            std::cout << "Buffer #" << i << ", Magnitude: " << mag << std::endl;
            expectLessThan (mag, 1.0e-4f, "Echoes were not flushed from delay line!");
        }

        plugin->releaseResources();
    }
};

static DelayClearTest delayClearTest;
