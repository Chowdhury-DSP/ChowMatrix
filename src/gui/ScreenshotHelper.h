#pragma once

// #define TAKE_SCREENSHOTS

#ifdef TAKE_SCREENSHOTS

#include "../ChowMatrix.h"

namespace ScreenshotHelper
{
     /** Take a series of screenshots used for the plugin documentation */
    void takeScreenshots (std::unique_ptr<ChowMatrix> plugin);

    /** Take a single screenshot for a given rectangle */
    void screenshotForBounds (Component* editor, Rectangle<int> bounds, const String& filename);

} // ScreenshotHelper

#endif // TAKE_SCREENSHOTS
