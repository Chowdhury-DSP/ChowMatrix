juce_add_console_app(ChowMatrix_headless
    PRODUCT_NAME "ChowMatrix"
    COMPANY_NAME chowdsp
)

juce_generate_juce_header(ChowMatrix_headless)
add_custom_command(TARGET ChowMatrix_headless
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E echo "copying $<TARGET_FILE:ChowMatrix_headless> to ${PROJECT_BINARY_DIR}/ChowMatrix"
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ChowMatrix_headless> ${PROJECT_BINARY_DIR}/ChowMatrix)

target_sources(ChowMatrix_headless PRIVATE
    main.cpp
    ScreenshotGenerator.cpp

    UnitTests/SaveLoadSpeedTest.cpp
    UnitTests/UnitTests.cpp
)

target_precompile_headers(ChowMatrix_headless PRIVATE ../pch.h)
target_include_directories(ChowMatrix_headless PRIVATE ../)

target_link_libraries(ChowMatrix_headless PUBLIC
    BinaryData
    ChowMatrix
)
