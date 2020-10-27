#include "MatrixAurora.h"

namespace
{
    static Colour purple (0xffb843c3);
    static Colour green (0xff38bb9d);

    constexpr float pixelMult= 0.6f;
    constexpr float lineWidth = 3.0f / pixelMult;
    constexpr float pixelWidth = 1.0f / pixelMult;

    constexpr float timerFreq = 20.0f;
    constexpr float omega_t = MathConstants<float>::twoPi / timerFreq;
}

inline float yCalc (float x, float t)
{
    auto xWave = 0.5f * std::sin (x) + 0.25f * std::sin (3.5f * x + 0.2f) + 0.125f * std::sin (10.1f * x - 0.5f);
    auto tWave = 1.0f + 0.4f * std::sin ((x + t) * omega_t * 2.0f + 0.3f) - 0.15f * std::sin ((x + t) * omega_t * 5.1f - 0.6f)
        + 0.05f * std::sin ((x + t) * omega_t * 12.0f - 1.2f);
    return xWave * tWave;
}

inline std::pair<float, float> rangeCalc (float x, float t, float intensity)
{
    auto env1 = std::pow (std::sin (MathConstants<float>::halfPi * x), 0.4f);
    auto env2 = std::pow (std::cos (MathConstants<float>::halfPi * x), 0.1f);
    auto env3 = 1.0f + std::exp (-std::pow(x - 0.3f, 2) / 0.02f);

    auto fullEnv = env1 * env2 * env3 * std::pow (std::sin (MathConstants<float>::halfPi * intensity), 2.4f);

    auto tEnv = 1.0f + 0.2f * std::sin (MathConstants<float>::pi * 4 * x + 0.3f);
    tEnv *= 1.0f - 0.2f * std::pow (std::sin ((x + t) * omega_t * 5.7f + 0.1f), 3)
        + 0.09f * std::pow (std::sin ((x + t) * omega_t * 29.4f + 0.9f), 3);

    auto bEnv = 0.5f + (x < 0.5f ? 0.0f : (x > 0.75f ? 3.0f : 3.0f * std::pow (4.0f * (x - 0.5f), 2)));
    bEnv *= 1.0f + 0.2f * std::pow (std::sin ((2 * x + t) * omega_t * 6.04f - 0.1f), 3)
        + 0.04f * std::pow (std::sin ((2 * x + t) * omega_t * 33.7f - 0.1f), 3);

    auto top = fullEnv * tEnv;
    auto bot = fullEnv * bEnv;

    return { 0.2f * top, 0.2f * bot };
}

inline std::pair<float, float> opaqueCalc (float x, float t, float intensity)
{
    auto xwave = 0.5f + 0.33f * std::sin (MathConstants<float>::pi * x) + 0.12f * std::sin (MathConstants<float>::pi * 3 * x);
    auto twave = 0.6f + 0.4f * std::pow (std::abs (std::sin (t * omega_t * 1.1f)), 0.6f);

    auto tEnv = 0.63f + 0.3f * std::sin ((2 * x + t) * omega_t * 4.7f - 0.1f) -
        0.07f * std::sin (3 * (x + t) * omega_t * 10.0f);

    auto bEnv = 0.94f + 0.06f * std::sin ((x + t) * 16.2f + 0.33f);
    
    auto topAlpha = jlimit (0.0f, 1.0f, xwave * twave * tEnv) * intensity;
    auto botAlpha = jlimit (0.0f, 1.0f, xwave * twave * bEnv) * intensity;

    return { topAlpha, botAlpha };
}

//==================================================================
MatrixAurora::MatrixAurora (std::atomic<float>* insanityParam) :
    insanityParam (insanityParam)
{
    setOpaque (false);
    setFramesPerSecond ((int) timerFreq);
    setInterceptsMouseClicks (false, false);
}

void MatrixAurora::update()
{
    time += (float) getMillisecondsSinceLastUpdate() / 1000.0f;

    for (auto& pt : points)
    {
        pt.y = yCalc (pt.x, time);
        pt.y_range = rangeCalc (pt.x, time, insanityParam->load());
        pt.opacities = opaqueCalc (pt.x, time, insanityParam->load());
    }
}

void MatrixAurora::paint (Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    for (auto& pt : points)
    {
        if (pt.y_range.first == 0 && pt.y_range.second == 0)
            continue;

        const auto line = pt.getLine (bounds);
        ColourGradient gr (Colours::transparentBlack, line.getStart(),
                           Colours::transparentBlack, line.getEnd(), false);

        gr.addColour (0.5 - 0.5 * double (pt.y_range.first),  green.withAlpha  (pt.opacities.first));
        gr.addColour (0.5 + 0.5 * double (pt.y_range.second), purple.withAlpha (pt.opacities.second));

        g.setGradientFill (gr);
        g.drawLine (line, lineWidth);
    }
}

void MatrixAurora::resized()
{
    const auto width = (float) getWidth();
    points.resize (static_cast<int> (width * pixelMult));

    for (size_t i = 0; i < points.size(); ++i)
        points[i].x = (float) i / (float) points.size();

    update();
}
