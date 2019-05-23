# WinSplit Revolution

WinSplit Revolution is a small utility which allows you to easily organize your
open windows by tiling, resizing and positioning them to make the best use of
your desktop real estate.

## History

This project recovers the code for v9.02 of WinSplit Revolution from the
[developpez](http://projets.developpez.com/projects/winsplit-revolution/) site.
Fixes and enhancements from versions 11.02, 11.03 and 11.04 have probably been
lost.

The main goal of this project is to get sizing and positioning working correctly
for windows 10. The [invisible frame](https://github.com/Maximus5/ConEmu/issues/284#issuecomment-257339519)
in windows 10 breaks sizing and positioning. A fix for this is already
implemented.

The secondary goal is to clean up, modernize and make the code base more
accessible for contributions.

## Building

### Build wxWidgets

- Open a developers x64 command prompt in the project folder
- `cd wxWidgets\build\msw`
- `nmake -f makefile.vc BUILD=release RUNTIME_LIBS=static TARGET_CPU=X64`

### Build WinSplit Revolution

- Open a developers x64 command prompt in the project folder
- `msbuild "Winsplit Revolution.sln" -property:Configuration=Release -property:Platform=x64`

## Supported Systems

This fork is only tested against windows 10 x64.
