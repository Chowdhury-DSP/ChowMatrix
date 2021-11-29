set_property(GLOBAL PROPERTY pluginval_setup)
function(create_pluginval_target target plugin)
    message(STATUS "Creating pluginval target for ${target}")

    if (APPLE)
        set(pluginval_url "https://github.com/Tracktion/pluginval/releases/latest/download/pluginval_macOS.zip")
        set(pluginval_exe pluginval/pluginval.app/Contents/MacOS/pluginval)
    elseif (WIN32)
        set(pluginval_url "https://github.com/Tracktion/pluginval/releases/latest/download/pluginval_Windows.zip")
        set(pluginval_exe pluginval/pluginval.exe)
    else ()
        set(pluginval_url "https://github.com/Tracktion/pluginval/releases/latest/download/pluginval_Linux.zip")
        set(pluginval_exe pluginval/pluginval)
    endif ()

    get_property(is_pluginval_setup GLOBAL PROPERTY pluginval_setup)
    if ("${is_pluginval_setup}" STREQUAL "yes")
        message(STATUS "Skipping pluginval setup...")
    else ()
        add_custom_target(download-pluginval)
        add_custom_command(TARGET download-pluginval
                POST_BUILD
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMAND cmake -E make_directory pluginval
                COMMAND curl -L ${pluginval_url} -o pluginval/pluginval.zip
                COMMAND cd pluginval && unzip -o pluginval.zip
                )
        add_custom_target(pluginval-all)
        set_property(GLOBAL PROPERTY pluginval_setup "yes")
    endif()

    set(name "${target}-pluginval")
    add_custom_target(${name})
    add_dependencies(${name} ${target})
    add_dependencies(${name} download-pluginval)
    get_target_property(plugin_location ${target} LIBRARY_OUTPUT_DIRECTORY)
    add_custom_command(TARGET ${name}
            POST_BUILD
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMAND ${pluginval_exe} --validate-in-process --strictness-level 8 --timeout-ms 600000 --output-dir "." --validate "${plugin_location}/${plugin}" || exit 1
            )
    add_dependencies(pluginval-all ${name})
endfunction()
