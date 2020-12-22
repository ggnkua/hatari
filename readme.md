# Highly opinionated Windows build system for Hatari plus Tat's debugger extensions
# (deal with it)

## How to build

1. We don't use cmake around these parts.
1. Install msys2/mingw32 from https://www.msys2.org. We had success downloading this:
https://repo.msys2.org/distrib/msys2-i686-latest.exe
   1. ~~FYI I installed mine on c:\m, and since there are hardcoded paths in the
build system as of now you might want to install there too. I'll fix the paths
soon in any case.~~
   1. Open a ```mingw32``` command line (not ```mingw64``` nor ```msys2```) and
in the prompt type ```pacman -S make mingw32/mingw-w64-i686-attica-qt5
mingw-w64-i686-gcc mingw-w64-i686-SDL2```. You might want to install git while
you're at it. I don't.
   1. Check your /usr/include folder for ```memory.h```. If such a file is not present then create one that contains the following single line: ```#include <string.h>```.
1. ~~Checkout this repository and switch to the proper branch using ```git
checkout debugger-extensions```.~~ ```master``` is now current
1. Open ```fastbuild\fbuild.bff``` in a text editor and change the 3 top
paths to reflect your setup. Change ```SRC_DIR``` to where the repository is,
GCC_BIN_DIR to the path where your gcc.exe is, and MINGW_ROOT to the root directory
of your MinGW install
1. In your mingw32 prompt ```cd``` to the repo's root directory and then type:
```
cd fastbuild
fbuild
```
(yes, we ship binaries with the source code. Deal.)

This should compile hatari at full speed (all CPU cores used).

6. If the above went well, then type the following:
```
cd ../tools/hrdb
qmake
make
```
This should build Tat's debugger GUI. If you want this to run outside a MinGW 
console (who doesn't?) type also the following: ```windeployqt.exe
release/```. Then you have the pleasure of copying 2347832462396 dlls from the
```mingw32/bin``` directory to the ```release``` folder. These did the trick
for me:
   * libgcc_s_dw2-1.dll
   * libstdc++-6.dll
   * libbrotlicommon.dll
   * libbrotlidec.dll
   * libharfbuzz-0.dll
   * libglib-2.0-0.dll
   * libfreetype-6.dll
   * libwinpthread-1.dll
   * libgraphite2.dll
   * libzstd.dll
   * libpcre2-16-0.dll
   * libicudt67.dll
   * libicuin67.dll
   * libicuuc67.dll
   * libpcre-1.dll
   * libbz2-1.dll
   * libdouble-conversion.dll
   * zlib1.dll
   * libpng16-16.dll

You should be done by now. Have fun!

