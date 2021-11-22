#include "PresetResaver.h"
#include "ChowMatrix.h"

PresetResaver::PresetResaver()
{
    this->commandOption = "--presets-resave";
    this->argumentDescription = "--presets-resave --dir=[PRESET DIR]";
    this->shortDescription = "Resaves factory presets";
    this->longDescription = "";
    this->command = std::bind (&PresetResaver::savePresets, this, std::placeholders::_1);
}

void PresetResaver::savePresets (const ArgumentList& args)
{
    File presetsDir = File::getCurrentWorkingDirectory().getChildFile ("res/presets");
    if (args.containsOption ("--dir"))
        presetsDir = args.getExistingFolderForOption ("--dir");

    if (! presetsDir.getChildFile ("Default.chowpreset").existsAsFile())
        ConsoleApplication::fail ("Incorrect presets directory! Unable to resave presets.");

    std::cout << "Resaving presets in directory: " << presetsDir.getFullPathName() << std::endl;

    std::unique_ptr<ChowMatrix> plugin (dynamic_cast<ChowMatrix*> (createPluginFilterOfType (AudioProcessor::WrapperType::wrapperType_Standalone)));
    auto& presetManager = plugin->getPresetManager();

    for (DirectoryEntry file : RangedDirectoryIterator (presetsDir, true))
    {
        auto presetFile = file.getFile();
        if (! presetFile.hasFileExtension ("chowpreset"))
        {
            std::cout << presetFile.getFullPathName() << " is not a preset file!" << std::endl;
            continue;
        }

        std::cout << "Resaving: " << presetFile.getFileName() << std::endl;

        // reset to default state
        presetManager.loadDefaultPreset();
        MessageManager::getInstance()->runDispatchLoopUntil (10); // pump dispatch loop so changes propagate...

        // load original preset
        auto originalPresetXml = XmlDocument::parse (presetFile);
        chowdsp::Preset originalPreset { originalPresetXml.get() };
        if (auto xmlState = originalPresetXml->getChildByName ("state"))
        {
            presetManager.loadPreset (originalPreset);
        }
        else
        {
            std::cout << "Unable to load preset!" << std::endl;
            continue;
        }

        MessageManager::getInstance()->runDispatchLoopUntil (10); // pump dispatch loop so changes propagate...

        // save as new preset
        auto curPreset = presetManager.getCurrentPreset();
        chowdsp::Preset newPreset (curPreset->getName(), curPreset->getVendor(), *curPreset->getState(), curPreset->getCategory());
        newPreset.toFile (presetFile);
    }
}
