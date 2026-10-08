# Building Infernal House 3DS

This document explains how to install the required tools, build
**Infernal House 3DS**, and produce files for the Nintendo 3DS.

The project is written in C and uses the **devkitPro / devkitARM**
toolchain, **libctru**, **Citro2D / Citro3D**, and **libvorbisidec** for
Ogg/Vorbis music playback. The build is driven by a GNU `Makefile`.

## 1. Requirements

You will need:

- **Git** and **GNU Make**;
- **devkitPro**, with the `3ds-dev` package group (devkitARM, libctru,
  Citro2D, Citro3D, `tex3ds`, etc.);
- **`3ds-libvorbisidec`**, for Ogg/Vorbis audio decoding;
- **`makerom`**, only to produce the installable `.cia` package;
- **`bannertool`**, only if you want to regenerate the HOME Menu banner.

The 3DS development tools are available on Linux, macOS, and Windows.
Follow the [official devkitPro
instructions](https://devkitpro.org/wiki/Getting_Started) to install its
package manager: `dkp-pacman` on Linux/macOS, or the environment
provided by devkitPro on Windows.

### Linux: GLIBC version requirement

Recent Linux builds of the devkitPro tools require **GLIBC 2.38 or
newer**. On an older distribution, some executables may refuse to start
even if the devkitPro packages are correctly installed.

To check the GLIBC version on your system:

``` sh
ldd --version
```

If your distribution does not provide GLIBC 2.38, **do not try to
upgrade GLIBC manually**: you can build the project using the official
[`devkitpro/devkitarm:latest`](https://hub.docker.com/r/devkitpro/devkitarm)
Docker image.

From the project root:

``` sh
docker run --rm -v "$(pwd):/work" -w /work devkitpro/devkitarm:latest make
```

The project directory is mounted into the container, so the generated
files are written directly to your host system. This command builds the
`.3dsx`; creating a `.cia` also requires `makerom` (see section 4).

### Linux (Debian/Ubuntu): automated installation

The repository provides a development environment installation script,
[`scripts/install-env.sh`](../scripts/install-env.sh). It configures the
devkitPro package repository, installs the required tools and libraries
as well as `makerom` and `bannertool`, and sets up the environment
variables.

After retrieving the sources (section 2), run the following from the
project root:

``` sh
sudo bash scripts/install-env.sh
source /etc/profile.d/devkit-env.sh
```

This script uses `apt` and requires administrator privileges. It does
**not** remove the GLIBC requirement: if your distribution is too old,
use Docker instead.

### Linux and macOS: manual installation

Once the devkitPro package manager is installed:

``` sh
sudo dkp-pacman -S 3ds-dev 3ds-libvorbisidec
```

On Windows, install the same packages from the MSYS2/devkitPro terminal
using `pacman` (without `sudo`):

``` sh
pacman -S 3ds-dev 3ds-libvorbisidec
```

On Linux, a standard installation uses the following variables:

``` sh
export DEVKITPRO=/opt/devkitpro
export DEVKITARM="$DEVKITPRO/devkitARM"
export PATH="$DEVKITPRO/tools/bin:$PATH"
```

The devkitPro installation normally configures these variables. If they
are missing, open a new session or check your environment configuration.
In particular, the `Makefile` requires `DEVKITARM` to be defined.

To check the tools:

``` sh
echo "$DEVKITPRO"
echo "$DEVKITARM"
command -v arm-none-eabi-gcc
command -v tex3ds
```

## 2. Getting the sources

Clone the repository, then change to its root directory:

``` sh
git clone https://github.com/Mithweth/infernal-house-3ds.git
cd infernal-house-3ds
```

All `make` commands shown below must be run from this directory, where
the `Makefile` is located.

## 3. Building the game (`.3dsx`)

A standard build requires a single command:

``` sh
make
```

It produces, among other files:

| File                  | Purpose                                                           |
|-----------------------|-------------------------------------------------------------------|
| `infernal-house.3dsx` | Executable for the **Homebrew Launcher** and compatible emulators |
| `infernal-house.smdh` | Application metadata and icon                                     |
| `infernal-house.elf`  | Intermediate executable, useful in particular for debugging       |

The `Makefile` also prepares the game resources. It generates the
`gfx.t3s` spritesheet descriptions, converts PNG images to `gfx.t3x`
textures using `tex3ds`, then builds the `romfs/` directory containing
the data required by the game: rooms, timelines, languages, sounds,
inventory, etc.

**You do not need to build the RomFS manually**: it is embedded into the
`.3dsx` during compilation.

### Cleaning and rebuilding

``` sh
make clean
make
```

`make clean` removes build files and generated resources without
deleting the source files under `resources/`.

### Building with `DEBUG`

The `Makefile` can enable the C `DEBUG` macro:

``` sh
make clean
make DEBUG=1
```

A static analysis target is also available:

``` sh
make lint
```

It uses GCC’s `-fanalyzer`; it does not replace testing the game on a
console or emulator.

## 4. Producing an installable package (`.cia`)

The `.cia` format can be used to install Infernal House directly into
the HOME Menu of a Nintendo 3DS with a suitable homebrew environment.

Generating the package requires **`makerom`**, which must be available
in the `PATH`. This tool can be obtained from the
[Project_CTR](https://github.com/3DSGuy/Project_CTR) project. It is not
required to build the `.3dsx`.

Build the game first, then generate the package:

``` sh
make
make cia
```

The result is:

``` text
infernal-house.cia
```

The HOME Menu banner is stored in `resources/cia/banner.bnr`. The
`Makefile` uses it as-is when creating the `.cia`: **`bannertool` is
therefore not required for a normal build**, as long as this file is
present.

If you modify `resources/cia/banner.png` or `resources/cia/banner.wav`,
you can regenerate the banner with:

``` sh
make banner
```

This command requires `bannertool` in the `PATH`. The resulting
`resources/cia/banner.bnr` file can then be used by `make cia`.

### Application version

The version is defined by the `APP_VERSION` variable in the `Makefile`
(default: `1.0.0`). To explicitly build a version:

``` sh
make clean
make APP_VERSION=1.0.0
make cia APP_VERSION=1.0.0
```

The same version is used both for the in-game display and for the CIA
package version information.

## 5. Testing the game

### With an emulator

The `.3dsx` file can be opened in a compatible Nintendo 3DS emulator
such as **Azahar**.

On Linux, if Azahar is installed through Flatpak, you can launch it from
the project root with:

``` sh
flatpak run --filesystem="$(pwd):ro" org.azahar_emu.Azahar "$(pwd)/infernal-house.3dsx"
```

The `--filesystem` option allows the emulator to read the project
directory for this invocation without permanently changing its Flatpak
permissions.

### On a Nintendo 3DS

There are two options:

- **`.3dsx`**: copy `infernal-house.3dsx` to the SD card, into a
  directory accessible from the **Homebrew Launcher** (for example
  `/3ds/infernal-house/`), then launch it from there.
- **`.cia`**: copy `infernal-house.cia` to the SD card, install it using
  a suitable title manager such as **FBI**, then launch it from the HOME
  Menu.

Both the `.3dsx` and `.cia` contain the RomFS resources required by the
game; you do not need to copy the `resources/` directory separately to
the SD card.

## 6. Common issues

| Symptom                                      | What to check                                                                                                            |
|----------------------------------------------|--------------------------------------------------------------------------------------------------------------------------|
| `Please set DEVKITARM in your environment`   | Check `DEVKITPRO` and `DEVKITARM`, then reopen your terminal after installing devkitPro.                                 |
| `arm-none-eabi-gcc: command not found`       | Check that the `3ds-dev` package group is installed and that the devkitPro environment is configured.                    |
| `tex3ds: command not found`                  | Check that the `3ds-dev` package group is installed and that the devkitPro tools are in the `PATH`.                      |
| `GLIBC_2.38 not found` (or a similar error)  | Your Linux distribution is too old for the installed devkitPro tools. Use the `devkitpro/devkitarm:latest` Docker image. |
| Linker error involving `vorbisidec` or `ogg` | Check that `3ds-libvorbisidec` and its dependencies are installed.                                                       |
| `makerom not found in PATH`                  | Install `makerom` to generate the `.cia`, or build only the `.3dsx` with `make`.                                         |
| `Missing resources/cia/banner.bnr`           | Regenerate the banner with `make banner` (requires `bannertool`).                                                        |
| Images or resources appear outdated          | Run `make clean`, then rebuild.                                                                                          |

## 7. Going further

Building the project and understanding the engine internals are two
separate topics. To learn about the project architecture and how to
modify rooms, interactions, timelines, or the interface, see
[docs/DEVELOPING.en.md](./DEVELOPING.en.md), which introduces the other
technical documentation.

For game controls and adventure mechanics, see
[HOWTOPLAY.en.md](./HOWTOPLAY.en.md).
