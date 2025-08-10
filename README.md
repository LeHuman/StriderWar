<!-- TITLE: StriderWar -->
<!-- KEYWORDS: game, graphics -->
<!-- LANGUAGES: C++, Python -->
<!-- TECHNOLOGY: Watcom  -->

<!-- LOGO -->
![Logo](<images/title.png>)

[![CI](https://github.com/lehuman/StriderWar/workflows/CI/badge.svg)](https://github.com/lehuman/StriderWar/actions)

[About](#about) - [Demo](#demo) - [Usage](#usage) - [Gameplay](#gameplay) - [Related](#related) - [License](#license)

## Status

<!-- STATUS -->
**`Prototype`**

## About
<!-- DESCRIPTION START -->
This is a game developed for the [IBM PC Jr](https://en.wikipedia.org/wiki/IBM_PCjr) with 128kb of RAM and joystick controls.
<!-- DESCRIPTION END -->

### Why

<!-- WHY START -->
I was always curious in developing for vintage hardware. I recently came across a cheap IBM PC Jr setup and decided to make something for it. Although there are more ways to squeeze way more out of this already underpowered machine, I decided to stick with a DOS app written in Watcom C++.

This project is more so an exercise in developing under a different set of constraints from what I am used to.
<!-- WHY END -->

## Demo

<!-- DEMO -->
![Demo](images/demo.gif)\
*Running on DOSBOX-X (So a bit faster than normal)*

## Usage

### Requirements

> [!NOTE]
> Due to the particular nature of the compiler, I have not attempted to compile / test on a host machine other than Windows. A lot of this information and the scripts provided are for a Windows host machine.

- [Open Watcom 2.0](https://github.com/open-watcom/open-watcom-v2/releases) >= 2025-05-24
  - We expect a default location for the install
- [VSCode](https://code.visualstudio.com/)
  - [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
  - [Tasks](https://marketplace.visualstudio.com/items?itemName=actboy168.tasks)
- [CMake](https://cmake.org/) >= 3.30
- [Python](https://www.python.org/) >= 3.13

- Running on Emulated hardware
  - [DOSBOX-X](https://dosbox-x.com/) >= 2025-05-03
    - I found DOSBOX-X The easiest to work with when it came to the IBM PC Jr
  - Game controller
    - I'm just using an XBox 360 Controller 🎮
  - OPTIONAL : For reading debug output
    - [Serial Monitoring App](https://github.com/LeHuman/ComMonitor-CLI)
    - [Tibbo Device Server Toolkit](https://tibbo.com/soi/software.html)
      - Setup up COM3 for DOSBOX-X to use and COM4 for your serial app. Just point them to the same endpoint to make them 'talk' to each other
      - Other virtual emulators might work, but I found this one to work best

- Running on hardware
  - [DOS 3.3](https://winworldpc.com/product/ms-dos/3x)
    - Not tested on anything higher
  - [SDCartJR](https://www.raphnet.net/electronique/sdcartJR/index_en.php) or some other way of loading and running files on the machine.
  - IBM PC Jr setup
    - Joysticks
    - Monitor /w speaker
    - Keyboard
    - The PC itself
    - Printer???

### Building

If you wish to enable debug output on COM3, modify `CMakeLists.txt` and set `DOSBOX_DEBUG` to `ON`

> [!WARNING]
> Enabling `DOSBOX_DEBUG` when running on target is not supported

1. Run the VSCode command (`Ctrl` + `lShift` + `P`) : `Cmake: Select a Kit`
2. Select `OpenWatcom v2 Beta (16-bit DOS)`
3. Run the VSCode command : `Cmake: Build`
   - Or press the ⚙ icon at the bottom
4. Output should be at `build/StriderWar.exe`

TODO: Media / Sprites

### Running

#### Emulated

A helper task is provided. Run the VSCode command `Tasks: Run Task` -> `Dosbox`.\
Or press the green icon on the bottom (if its there).\
Or run the following script.

``` ps
./scripts/run.ps1
```

#### On Target

1. Make sure to rename the executable to something shorter like "SWAR"
2. Copy all of the related files and executable to the same folder onto the target device
3. Run the following command

```bat
SWAR
```

## Gameplay

TODO: Game rules and controls

## Related

- [Technical Reference Manual](https://bitsavers.trailing-edge.com/pdf/ibm/pc/pc_jr/PCjr_Technical_Reference_Nov83.pdf)
- int10h/[ibm-5153-color-true-cga-palette](https://int10h.org/blog/2022/06/ibm-5153-color-true-cga-palette/)
- maizure/[decoded-sopwith](https://www.maizure.org/projects/decoded-sopwith/index.html)
- ricardoquesada/[bios-8088](https://github.com/ricardoquesada/bios-8088)

## License

Licensed under MIT license
   ([LICENSE-MIT](LICENSE-MIT) or <http://opensource.org/licenses/MIT>)
