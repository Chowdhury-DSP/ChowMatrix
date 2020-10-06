#!/bin/bash

plugin_name=$1
echo "Creating plugin ${plugin_name}..."

echo "Copying source files..."
mv src/TempPlugin.h src/${plugin_name}.h
mv src/TempPlugin.cpp src/${plugin_name}.cpp

echo "Setting up source files..."

declare -a source_files=("validate.sh" "win_builds.sh" "CMakeLists.txt" "src/CMakeLists.txt" "src/${plugin_name}.h" "src/${plugin_name}.cpp")
for file in "${source_files[@]}"; do
    sed -i.bak -e "s/TempPlugin/${plugin_name}/g" $file
done

rm *.bak
rm */*.bak

