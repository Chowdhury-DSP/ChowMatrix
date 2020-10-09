# CHOW Matrix

[![License](https://img.shields.io/badge/License-BSD-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

This repository contains template code for building a Chowdhury DSP
audio plugin.

## Roadmap

- [ ] Basic GUI Layout
  - [ ] Top Bar
  - [ ] Bottom Bar
  - [ ] Main View
  - [ ] Insanity slider
  - [ ] Detail View
- [ ] Basic matrix node
  - [ ] Delay
  - [ ] Panner
  - [ ] Gain
  - [ ] Feedback
- [ ] Node graph view
  - [ ] Node circles w/ click and drag
  - [ ] Graph connections
  - [ ] Details tooltip
- [ ] Scroll view
  - [ ] Scroll bar
  - [ ] Node circles
  - [ ] Details boxes

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
