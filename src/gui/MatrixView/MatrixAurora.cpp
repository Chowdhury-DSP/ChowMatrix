#include "MatrixAurora.h"

namespace
{
const String graphicsFilePath = "ChowdhuryDSP/ChowMatrix/GraphicsConfig.txt";

static Colour purple (0xffb843c3);
static Colour green (0xff38bb9d);

constexpr float pixelMult = 0.51f;
constexpr float lineWidth = 4.0f / pixelMult;

constexpr float insanityFloor = 0.15f;
constexpr float timerFreq = 18.0f;
constexpr float omega_t = MathConstants<float>::twoPi / timerFreq;
} // namespace

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
    auto env3 = 1.0f + std::exp (-std::pow (x - 0.3f, 2) / 0.02f);

    auto fullEnv = env1 * env2 * env3 * std::pow (std::sin (MathConstants<float>::halfPi * intensity), 2.4f);

    auto tEnv = 1.0f + 0.2f * std::sin (MathConstants<float>::pi * 4 * x + 0.3f);
    tEnv *= 1.0f - 0.2f * std::pow (std::sin ((x + t) * omega_t * 5.7f + 0.1f), 3.0f)
            + 0.09f * std::pow (std::sin ((x + t) * omega_t * 29.4f + 0.9f), 3.0f);

    auto bEnv = 0.5f + (x < 0.5f ? 0.0f : (x > 0.75f ? 3.0f : 3.0f * std::pow (4.0f * (x - 0.5f), 2.0f)));
    bEnv *= 1.0f + 0.2f * std::pow (std::sin ((2 * x + t) * omega_t * 6.04f - 0.1f), 3.0f)
            + 0.04f * std::pow (std::sin ((2 * x + t) * omega_t * 33.7f - 0.1f), 3.0f);

    auto intensityScale = 1.0f + 1.5f * std::pow (intensity, 1.5f);
    auto top = jlimit (0.0f, 10.0f, float (fullEnv * tEnv * intensityScale));
    auto bot = jlimit (0.0f, 10.0f, float (fullEnv * bEnv * intensityScale));

    return { 0.2f * top, 0.2f * bot };
}

inline std::pair<float, float> opaqueCalc (float x, float t, float intensity)
{
    auto xwave = 0.5f + 0.33f * std::sin (MathConstants<float>::pi * x) + 0.12f * std::sin (MathConstants<float>::pi * 3 * x);
    auto twave = 0.6f + 0.4f * std::pow (std::abs (std::sin (t * omega_t * 1.1f)), 0.6f);

    auto tEnv = 0.63f + 0.3f * std::sin ((2 * x + t) * omega_t * 4.7f - 0.1f) - 0.07f * std::sin (3 * (x + t) * omega_t * 10.0f);

    auto bEnv = 0.94f + 0.06f * std::sin ((x + t) * 16.2f + 0.33f);

    auto topAlpha = jlimit (0.0f, 1.0f, xwave * twave * tEnv) * intensity;
    auto botAlpha = jlimit (0.0f, 1.0f, xwave * twave * bEnv) * intensity;

    return { topAlpha, botAlpha };
}

//==================================================================
File getGraphicsConfigFile()
{
    File graphicsConfigFile = File::getSpecialLocation (File::userApplicationDataDirectory);
    graphicsConfigFile = graphicsConfigFile.getChildFile (graphicsFilePath);

    if (! graphicsConfigFile.existsAsFile())
    {
        graphicsConfigFile.create();
        graphicsConfigFile.appendText ("ThrottleGraphics:FALSE\n");
    }

    return graphicsConfigFile;
}

MatrixAurora::MatrixAurora (std::atomic<float>* insanityParam) : insanityParam (insanityParam)
{
    setOpaque (false);
    setFramesPerSecond ((int) timerFreq);
    setInterceptsMouseClicks (false, false);

    auto graphicsFile = getGraphicsConfigFile();
    refreshGraphicsSettings (graphicsFile);
}

void MatrixAurora::update()
{
    if (insanityParam->load() < insanityFloor)
        setFramesPerSecond (1);
    else if (throttleGraphics)
        setFramesPerSecond (4);
    else
        setFramesPerSecond (1 + (int) (std::pow (insanityParam->load(), 0.2f) * (timerFreq - 1.0f)));
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
    if (insanityParam->load() < insanityFloor) // don't update if not needed
        return;

    auto bounds = getLocalBounds().toFloat();

    for (auto& pt : points)
    {
        if (pt.y_range.first == 0 && pt.y_range.second == 0)
            continue;

        const auto line = pt.getLine (bounds);
        ColourGradient gr (Colours::transparentBlack, line.getStart(), Colours::transparentBlack, line.getEnd(), false);

        gr.addColour (0.35, green.withAlpha (pt.opacities.first));
        gr.addColour (0.65, purple.withAlpha (pt.opacities.second));

        g.setGradientFill (gr);
        g.drawLine (line, lineWidth);
    }
}

void MatrixAurora::resized()
{
    const auto width = (float) getWidth();
    points.resize (static_cast<size_t> (jmin (width * pixelMult, 450.0f)));

    for (size_t i = 0; i < points.size(); ++i)
        points[i].x = (float) i / (float) points.size();

    update();
}

void MatrixAurora::setGraphicsThrottle (bool shouldThrottle)
{
    auto graphicsFile = getGraphicsConfigFile();
    graphicsFile.deleteFile();
    graphicsFile.create();

    if (shouldThrottle)
        graphicsFile.appendText ("ThrottleGraphics:TRUE\n");
    else
        graphicsFile.appendText ("ThrottleGraphics:FALSE\n");

    refreshGraphicsSettings (graphicsFile);
}

void MatrixAurora::refreshGraphicsSettings (File& graphicsFile)
{
    StringArray lines;
    graphicsFile.readLines (lines);

    auto shouldThrottle = lines[0].fromLastOccurrenceOf ("ThrottleGraphics:", false, false);
    throttleGraphics = shouldThrottle.contains ("TRUE");
}
