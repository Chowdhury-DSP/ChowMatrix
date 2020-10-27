#pragma once

#include <JuceHeader.h>

class MatrixAurora : public AnimatedAppComponent
{
public:
    MatrixAurora (std::atomic<float>* insanityParam);

    void update() override;
    void paint (Graphics& g) override;
    void resized() override;

private:
    struct AuroraPt
    {
        float x;
        float y = 0.0f;
        std::pair<float, float> y_range { 0.0f, 0.0f };
        std::pair<float, float> opacities { 0.0f, 0.0f };

        inline Line<float> getLine (const Rectangle<float>& b) const noexcept
        {
            auto xPos = x * b.getWidth();
            const auto halfHeight = b.getHeight() / 2.0f;
            auto yTop = halfHeight - (y + y_range.first)  * halfHeight;
            auto yBot = halfHeight - (y - y_range.second) * halfHeight;
            return { xPos, yTop, xPos, yBot };
        }
    };

    std::vector<AuroraPt> points;

    float time = 0.0f;
    std::atomic<float>* insanityParam = nullptr;    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MatrixAurora)
};
