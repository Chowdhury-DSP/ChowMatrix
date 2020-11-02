# CHOW Matrix

[![Build Status](https://travis-ci.com/Chowdhury-DSP/ChowMatrix.svg?token=Ub9niJrqG1Br1qaaxp7E&branch=main)](https://travis-ci.com/Chowdhury-DSP/ChowMatrix)
[![License](https://img.shields.io/badge/License-BSD-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

This repository contains template code for building a Chowdhury DSP
audio plugin.

## TODO

Features:
- [ ] Presets
- [x] Delay line modes
- [x] Create nodes with double-click
- [x] Linked controls
- [x] Randomise button
- [ ] Auto-updating

Maybe:
- [ ] Change create nodes to ctrl-click or alt-click...
- [ ] When node is deleted pass on children to parent
- [ ] Delay line mixing

## Building

To build from scratch, you must have CMake installed.

```bash
# Clone the repository
$ git clone https://github.com/Chowdhury-DSP/ChowMatrix.git
$ cd ChowMatrix

# initialize and set up submodules
$ git submodule update --init --recursive

# build with CMake
$ cmake -Bbuild
$ cmake --build build --config Release
```

## License

ChowMatrix is open source, and is licensed under the BSD 3-clause license.
Enjoy!
