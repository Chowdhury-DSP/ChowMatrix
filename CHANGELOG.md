# Changelog

All notable changes to this project will be documented in this file.

## UNRELEASED
- Added "Alien" delay mode.
- Added reverse delay parameter.
- Added lock/unlock options for parameter randomisation.
- Fixed "hang" when bringing delay to 0 ms in "Analog" mode.
- Improved performance for pan modulation.
- Added option to throttle graphics rendering for the Matrix View.
- Improved memory usage.

## [1.2.0] 2021-04-23
- Added new delay modes:
  - Liquid (Sinc interpolation)
  - Super Liquid (more ideal Sinc interpolation)
  - Lo-Fi (4096-stage BBD delay emulation)
  - Analog (16384-stage BBD delay emulation)
- Added parameter modulation targets for host control.
- Added insanity reset/insanity lock behaviour.
- Support for Macs with Apple Silicon.
- Created installer packages for Windows/Mac/Linux.

## [1.1.0] 2021-01-13
- Added individual node modulation, for delay time and panning.
- Added pitch-shifting and diffusion for each delay node.
- Added LV2 compilation for Linux.
- Made Graph View scrollable.
- Added A/B buttons for switching plugin state.
- Added individual node colouring.
- Extended range for tempo-synced delay times.
- Fixed node distortion bug in Logic/Garageband.
- Fixed bug with incorrect delay times between sibling nodes.
- Fixed preset menu not returning to currently selected preset.

## [1.0.0] 2020-11-25
- Initial release.
