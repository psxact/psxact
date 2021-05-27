![Logo](images/logo.png)

## Goals

1. Emulate the PlayStation&trade; 1 console as accurately and efficiently as possible.
2. All behaviors verified with a [suite of hardware tests][hw-tests].
3. Clean and simple code.

## Current Status

Currently, the emulator is capable of booting a small amount of commercial games.

## Usage

The emulator requires a BIOS file, which can be dumped from a physical console.

Once you have a BIOS file, PSXACT can be invoked via command line, using the following simple interface:

```
$ psxact -b <bios file here> -g <game file here>
```

## Building

This project uses `make` for building, and relies on [SDL2][sdl2] for multimedia. At the very simplest, building is accomplished by invoking `make` from the root of the project.

## Contributing

If you'd like to contribute, please create a fork and issue pull requests! Newcomers and experts alike are welcome to contribute.

## Communication

Join us on our `freenode` channel, `#psxact`!

[hw-tests]: https://gitlab.com/psx-emu-dev/psx-hardware-tests
[sdl2]: https://www.libsdl.org/
