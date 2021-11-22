#include "PresetManager.h"

namespace
{
const String userPresetPath = "ChowdhuryDSP/ChowMatrix/UserPresets.txt";
}

PresetManager::PresetManager (AudioProcessorValueTreeState& thisVts, StateManager& stateMgr) : chowdsp::PresetManager (thisVts), stateManager (stateMgr)
{
    setUserPresetConfigFile (userPresetPath);
    setDefaultPreset (chowdsp::Preset { BinaryData::Default_chowpreset, BinaryData::Default_chowpresetSize });

    std::vector<chowdsp::Preset> factoryPresets;
    factoryPresets.emplace_back (BinaryData::Chord_chowpreset, BinaryData::Chord_chowpresetSize);
    factoryPresets.emplace_back (BinaryData::Crazy_chowpreset, BinaryData::Crazy_chowpresetSize);
    factoryPresets.emplace_back (BinaryData::Lush_chowpreset, BinaryData::Lush_chowpresetSize);
    factoryPresets.emplace_back (BinaryData::Rhythmic_chowpreset, BinaryData::Rhythmic_chowpresetSize);
    factoryPresets.emplace_back (BinaryData::Wobble_chowpreset, BinaryData::Wobble_chowpresetSize);
    factoryPresets.emplace_back (BinaryData::Wonky_chowpreset, BinaryData::Wonky_chowpresetSize);
    addPresets (factoryPresets);

    loadDefaultPreset();

#if JUCE_IOS
    File appDataDir = File::getSpecialLocation (File::userApplicationDataDirectory);
    auto userPresetFolder = appDataDir.getChildFile (userPresetPath).getSiblingFile ("Presets");
    if (! userPresetFolder.isDirectory())
    {
        userPresetFolder.deleteFile();
        userPresetFolder.createDirectory();
    }

    setUserPresetPath (userPresetFolder);
#endif // JUCE_IOS
}

std::unique_ptr<XmlElement> PresetManager::savePresetState()
{
    return stateManager.saveState();
}

void PresetManager::loadPresetState (const XmlElement* xml)
{
    stateManager.loadState (xml);
}

chowdsp::Preset PresetManager::loadUserPresetFromFile (const File& file)
{
    chowdsp::Preset compatiblePreset { file };
    if (compatiblePreset.isValid())
        return std::move (compatiblePreset);

    auto xml = XmlDocument::parse (file);
    if (xml == nullptr)
        return compatiblePreset;

    if (xml->getTagName() != chowdsp::Preset::presetTag.toString())
        return compatiblePreset;

    auto name = xml->getStringAttribute (chowdsp::Preset::nameTag);
    if (name.isEmpty())
        return compatiblePreset;

    auto vendor = xml->getStringAttribute (chowdsp::Preset::vendorTag);
    if (vendor.isEmpty())
    {
        vendor = name.upToFirstOccurrenceOf ("_", false, false);
        name = name.fromLastOccurrenceOf ("_", false, false);
    }

    auto category = xml->getStringAttribute (chowdsp::Preset::categoryTag);

    auto* xmlState = xml->getChildByName (StateManager::stateXmlTag);
    if (xmlState == nullptr)
        return compatiblePreset;

    return { name, vendor, *xmlState, category };
}
