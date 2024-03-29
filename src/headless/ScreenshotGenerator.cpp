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

    std::unique_ptr<AudioProcessor> plugin (createPluginFilterOfType (AudioProcessor::WrapperType::wrapperType_Standalone));
    plugin->setCurrentProgram (2); // set to "crazy" preset
    MessageManager::getInstance()->runDispatchLoopUntil (1000);

    std::unique_ptr<AudioProcessorEditor> editor (plugin->createEditorIfNeeded());
    MessageManager::getInstance()->runDispatchLoopUntil (1000);

    editor->setSize (700, 700); // make editor larger
    screenshotForBounds (editor.get(), editor->getLocalBounds(), outputDir, "full_gui.png");
    screenshotForBounds (editor.get(), { 0, 370, editor->getWidth(), 260 }, outputDir, "DetailsView.png");

    editor->setSize (700, 850);
    screenshotForBounds (editor.get(), { 0, 45, editor->getWidth(), 352 }, outputDir, "GraphView.png");
    screenshotForBounds (editor.get(), { 0, 810, editor->getWidth(), 40 }, outputDir, "BottomBar.png");

    plugin->editorBeingDeleted (editor.get());
}

void ScreenshotGenerator::screenshotForBounds (Component* editor, Rectangle<int> bounds, const File& dir, const String& filename)
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
