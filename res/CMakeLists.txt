juce_add_binary_data(BinaryData SOURCES
    gui.xml
    Diceroll.png
    Panic.png
    home.svg

    presets/Crazy.chowpreset
    presets/Default.chowpreset
    presets/Lush.chowpreset
    presets/Rhythmic.chowpreset
)

# Need to build BinaryData with -fPIC flag on Linux
set_target_properties(BinaryData PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)
