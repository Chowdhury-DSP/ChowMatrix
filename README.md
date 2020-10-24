# CHOW Matrix

[![License](https://img.shields.io/badge/License-BSD-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

This repository contains template code for building a Chowdhury DSP
audio plugin.

## Roadmap

- [x] Basic GUI Layout
  - [x] Top Bar
  - [x] Bottom Bar
  - [x] Main View
  - [x] Insanity slider
  - [x] Detail View
  - [x] Tooltip view
- [x] Matrix node
  - [x] Delay
  - [x] Panner
  - [x] Gain
  - [x] Feedback
- [x] Node graph view
  - [x] Node circles w/ click and drag
  - [x] Graph connections
  - [x] Details tooltip
- [x] Scroll view
  - [x] Scroll bar
  - [x] Node circles
  - [x] Details boxes
  - [x] Parameter labels next to scroll view
- [ ] GUI audit
  - [x] Colours
  - [ ] Fonts
  - [ ] Layout
- [ ] Insanity
  - [x] Slider
  - [ ] Algorithm
  - [ ] Aurora....
- [ ] Utilities
  - [x] save/load
  - [ ] presets
- [ ] Extras
  - [ ] Delay line mixing
  - [ ] Delay line modes...
  - [ ] Auto-updating

TESTING!!!!

## Building

To build from scratch, you must have CMake installed.

```bash
# Clone the repository
$ git clone https://github.com/Chowdhury-DSP/ChowMatrix.git
$ cd ChowMatrix

# initialize and set up submodules
$ git submodule update --init --recursive

# set up plugin
$ ./setup.sh ChowMatrix

# build with CMake
$ cmake -Bbuild
$ cmake --build build --config Release
```

## License

ChowMatrix is open source, and is licensed under the BSD 3-clause license.
Enjoy!
