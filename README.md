[![Build Status](https://travis-ci.org/adam-becker/psxact.svg?branch=develop)](https://travis-ci.org/adam-becker/psxact)
[![Build status](https://ci.appveyor.com/api/projects/status/drk4b45g4pyij3ij?svg=true)](https://ci.appveyor.com/project/adam-becker/psxact)

![Logo](images/logo.png)

This project aims to emulate the PlayStation&trade; 1 console in an accurate
fashion. No hacks or shortcuts will be used to accomplish this goal. Everything
needed will be provided by default, this means there aren't any plug-ins to
locate, maintain, or configure.

## Current Status

Currently, the emulator is capable of booting a small amount of commercial
games with compatibility constantly evolving.

![Current status](images/current.png)

## Usage

The emulator requires a BIOS file, which can be dumped from a physical console.

Once you have a BIOS file, PSXACT can be invoked via command line, using the
following simple interface:

```
$ psxact <bios file here> <game file here>
```

## Building

This project uses CMake for builds, and requires SDL2.

## Contributing

If you'd like to contribute, please create a fork and issue pull requests!
Newcomers and experts alike are welcome to [.github/CONTRIBUTING.md](contribute).

## Communication

Join us on our `freenode` channel, `#psxact`!
