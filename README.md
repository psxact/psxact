# PSXACT
## Cycle-accurate original PlayStation emulator

Hello, and welcome to the PSXACT GitHub repository! For years, I've had a huge
fascination with emulation. I've written many emulators, from NES and GB/C
to SNES and GBA. I've always wanted to make an emulator for a 3D capable
system, but I could never decide on one.

Years passed, and I made minor adjustments to my already working emulators,
and I decided that I would finally set out and create an emulator for a 3D
system. Inspired by the work that MarathonMan has been doing with his emulator
[CEN64](http://www.cen64.com), I decided to target the main rival of that generation; the PS1.

With it's small, but capable CPU (the MIPS R3051), vector math coprocessor,
and 3D GPU, It seemed to be exactly the kind of challenge I've been after...

Then I found the documentation. To say that it is lacking would be an
understatement. "Where is all the low-level documentation of the different
components of the PS1?", I searched and searched, but alas...nothing.

It is my full intention to change that. I plan to run tests on real
hardware, and fully document my findings. From how the data cache fully
operates, to what each command in the GPU really does from one cycle to
the next.

I also plan to implement each component as close to real life as possible.
That means no shortcuts when it comes to the CD-ROM controller, or game
specific hacks to get another game working. If a game doesn't work, that
only means my emulation of the components is not accurate enough, and requires
a better understanding.

## Building

### Visual Studio 2015

* Open `psxact.sln`
* Build
* Done!

### Other compilers

Other compilers aren't currently supported.

## Contributing

If you'd like to contribute, please create a fork and issue pull requests! I am
very open to newcomers, and will need all the help I can get to make the best
PS1 emulator available.

## I found a bug!

That's great! Please report any and all bugs using the project issue
tracker. Be as precise as possible so that the bug can be found easier.
