juce_add_binary_data(BinaryData SOURCES
    gui.xml
    gui_ios.xml
    
    Diceroll.png
    Panic.png
    home.svg
    undo-solid.png
    bars-solid.svg
    cog-solid.svg
    LeftArrow.svg
    RightArrow.svg
    ellipsis-h-solid.png

    presets/Crazy.chowpreset
    presets/Default.chowpreset
    presets/Lush.chowpreset
    presets/Rhythmic.chowpreset
    presets/Chord.chowpreset
    presets/Wobble.chowpreset
    presets/Wonky.chowpreset
)

# Need to build BinaryData with -fPIC flag on Linux
set_target_properties(BinaryData PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)
