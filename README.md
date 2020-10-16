# CHOW Matrix

[![License](https://img.shields.io/badge/License-BSD-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

This repository contains template code for building a Chowdhury DSP
audio plugin.

## Roadmap

- [ ] Basic GUI Layout
  - [x] Top Bar
  - [ ] Bottom Bar
  - [x] Main View
  - [ ] Insanity slider
  - [ ] Detail View
  - [ ] Tooltip view
- [ ] Matrix node
  - [x] Delay
  - [x] Panner
  - [ ] Gain
  - [ ] Feedback
- [x] Node graph view
  - [x] Node circles w/ click and drag
  - [x] Graph connections
  - [x] Details tooltip
- [ ] Scroll view
  - [ ] Scroll bar
  - [ ] Node circles
  - [ ] Details boxes
- [ ] Utilities
  - [ ] save/load
  - [ ] presets
- [ ] Extras
  - [ ] Spaghetti mode
  - [ ] Auto-updater

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
