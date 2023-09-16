# nCHIP-8
Yet another CHIP-8 interpreter, written in C++17 with the use of SDL2 (as always) and Dear ImGui for UI. But it's customizable and has some debug capabilities!

[**Video demonstration**](https://drive.google.com/file/d/1iOsX1t5adUIJu2qDHnsUx5Pd6OYt5gCg/view?usp=sharing)

[**Screenshots**](screenshots/)

It wasn't written for educational purposes, but just for fun. Nevertheless, I still learned something new.

## Features
- Sound support (Waveforms: sine, square and saw)
    - Changeable frequency and dB level
- Display colors are in RGB and they can be changed
- Two keypad mappings: the original COSMAC VIP and the modern one (which is used by most other interpreters)
- You can set custom CPU frequency value
- You can change the RANDOM seed (useful for debugging)
- If some games don't have mood to function properly, you can try to make them feel better by touching these quirks:
    - The `BNNN` instruction: use V0 as the offset
    - `DXYN`: wrap pixels instead of clipping them
    - `8XY6` & `8XYE`: set VX to VY
    - `8XY1`, `8XY2` and `8XY3`: reset VF
    - `FX55` & `FX65`: increment the I register
- Debug capabilities:
    - Disassembler
    - Set, edit and remove breakpoints (they can also be named)
    - View stack
    - View and modify registers
    - Instruction executor
    - Keypad, in which buttons are held until they are released by second click (useful in the STEP mode)

## Building
You can build simply by creating `build` directory, going to it and calling the cmake (e.g. `cmake ..`)

Optionally, you can install my interpreter typing something like `sudo cmake --install .`

## Usage
Just type `./nchip8` (or `nchip8` if you've installed it)!

Maybe, for certain games you'd like to change the amount of cycles per second, colors, etc.

## Todo
- [ ] Ability to optionally disable flickering
- [ ] Support for SCHIP
- [ ] Support for XO-CHIP
- [ ] More debug features
    - [ ] Watch window
    - [ ] Memory view (and editor)
    - [ ] Logger
    - [ ] Add more controls to the existing tools
- [ ] CLI options
- [ ] Some kind of library that has a list of games, and for each game one can specify quirks and other options.

## Used resources
[**Tobias' guide**](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)

[**Wikipedia article**](https://en.wikipedia.org/wiki/CHIP-8)

[**CHIP-8 test suite**](https://github.com/Timendus/chip8-test-suite)

[**Instruction set reference**](https://chip8.gulrak.net/)

[**List of most known CHIP-8 extensions**](https://gist.github.com/tobiasvl/a0e43bdcd64d9ef6be35f5c67fcff0dd)

[**Collections of ROMs for CHIP-8**](https://github.com/kripod/chip8-roms)
