#include "ScreenshotHelper.h"

#ifdef TAKE_SCREENSHOTS

namespace ScreenshotHelper
{

void takeScreenshots (std::unique_ptr<ChowMatrix> plugin)
{
    plugin->getPresetManager().setPreset (3); // set to "crazy" preset
    std::unique_ptr<AudioProcessorEditor> editor (plugin->createEditorIfNeeded());
    editor->setSize (700, 700); // make editor larger
    
    screenshotForBounds (editor.get(), editor->getLocalBounds(), "full_gui.png");
    screenshotForBounds (editor.get(), { 0, 35, editor->getWidth(), 325 }, "GraphView.png");
    screenshotForBounds (editor.get(), { 0, 400, editor->getWidth(), 230 }, "DetailsView.png");

    plugin->editorBeingDeleted (editor.get());
}

File getScreenshotFolder()
{
    return File ("D:/ChowDSP/Plugins/MatrixDelay/manual/screenshots");
}

void screenshotForBounds (Component* editor, Rectangle<int> bounds, const String& filename)
{
    auto screenshot = editor->createComponentSnapshot (bounds);

    File pngFile = getScreenshotFolder().getChildFile (filename);
    pngFile.deleteFile();
    pngFile.create();
    auto pngStream = pngFile.createOutputStream();

    if (pngStream->openedOk())
    {
        PNGImageFormat pngImage;
        pngImage.writeImageToStream (screenshot, *pngStream.get());
    }
}

} // ScreenshotHelper

#endif // TAKE_SCREENSHOTS
