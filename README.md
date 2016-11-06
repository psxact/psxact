# PSXACT
## Cycle-accurate PlayStation emulator

This project is a work in progress, and currently is in the process of booting
through the `SCPH1001 BIOS`.

The emulator expects a `bios.rom` file to be in the same directory, this file
can be dumped from your real PlayStation&trade;. I have only implemented a
software rasterizer for graphics, this is obviously fairly demanding.

I will not be optimizing or accepting optimization PRs until the core is
accurate. There are plenty of other very fast, very inaccurate emulators
available, use one of them if you wish.

## Current Status

Currently, the emulator hangs after drawing the Sony&trade; logo while trying
to communicate with the CD-ROM drive. Textured primitives are partially
implemented.

![Current status](images/current.png)

## Building

This project uses CMake for builds, and requires SDL2.

## Contributing

If you'd like to contribute, please create a fork and issue pull requests! I am
very open to newcomers, and will need all the help I can get to make the best
PS1 emulator available.

## I found a bug!

That's great! Please report any and all bugs using the project issue
tracker. Be as precise as possible so that the bug can be found easier.
