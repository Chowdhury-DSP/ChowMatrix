#include "ScreenshotGenerator.h"
#include "../ChowMatrix.h"

ScreenshotGenerator::ScreenshotGenerator()
{
    this->commandOption = "--screenshots";
    this->argumentDescription = "--screenshots --out=[DIR]";
    this->shortDescription = "Generates screenshots for ChowMatrix documentation";
    this->longDescription = "";
    this->command = std::bind (&ScreenshotGenerator::takeScreenshots, this, std::placeholders::_1);
}

void ScreenshotGenerator::takeScreenshots (const ArgumentList& args)
{
    File outputDir = File::getCurrentWorkingDirectory();
    if (args.containsOption ("--out"))
        outputDir = args.getExistingFolderForOption ("--out");

    std::cout << "Generating screenshots... Saving to " << outputDir.getFullPathName() << std::endl;
    
    auto plugin = std::make_unique<ChowMatrix>();
    plugin->getStateManager().getPresetManager().setPreset (3); // set to "crazy" preset
    std::unique_ptr<AudioProcessorEditor> editor (plugin->createEditorIfNeeded());
    editor->setSize (700, 700); // make editor larger
    
    screenshotForBounds (editor.get(),           editor->getLocalBounds() , outputDir, "full_gui.png");
    screenshotForBounds (editor.get(), { 0, 35,  editor->getWidth(), 325 }, outputDir, "GraphView.png");
    screenshotForBounds (editor.get(), { 0, 400, editor->getWidth(), 230 }, outputDir, "DetailsView.png");

    plugin->editorBeingDeleted (editor.get());
}

void ScreenshotGenerator::screenshotForBounds (Component* editor, Rectangle<int> bounds,
                                               const File& dir, const String& filename)
{
    auto screenshot = editor->createComponentSnapshot (bounds);

    File pngFile = dir.getChildFile (filename);
    pngFile.deleteFile();
    pngFile.create();
    auto pngStream = pngFile.createOutputStream();

    if (pngStream->openedOk())
    {
        PNGImageFormat pngImage;
        pngImage.writeImageToStream (screenshot, *pngStream.get());
    }
}
