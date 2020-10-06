# JUCE Plugin Template

[![Build Status](https://travis-ci.com/Chowdhury-DSP/JUCEPluginTemplate.svg?token=Ub9niJrqG1Br1qaaxp7E&branch=master)](https://travis-ci.com/Chowdhury-DSP/JUCEPluginTemplate)
[![License](https://img.shields.io/badge/License-BSD-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

This repository contains template code for building a Chowdhury DSP
audio plugin.

## Building

To build from scratch, you must have CMake installed.

```bash
# Clone the repository
$ git clone https://github.com/Chowdhury-DSP/JUCEPluginTemplate.git
$ cd JUCEPluginTemplate

# initialize and set up submodules
$ git submodule update --init --recursive

# set up plugin
$ ./setup.sh MyPluginName

# build with CMake
$ cmake -Bbuild
$ cmake --build build --config Release
```

## License

JUCEPluginTemplate is open source, and is licensed under the BSD 3-clause license.
Enjoy!
