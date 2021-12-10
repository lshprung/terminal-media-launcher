# Windows Compilation Instructions

Compiling on Windows requires access to the MinGW toolchain. The easiest way to get the necessary libraries is by installing [MSYS2](https://www.msys2.org/) which provides a package manager: pacman.

1. Download the latest version of MSYS2 from [here](https://www.msys2.org/).

2. Open the Start Menu and launch "MSYS2 MinGW 64-bit" to open a terminal window.

3. Run `pacman -Syu` until all packages are up to date

4. `pacman -S git mingw-w64-x86_64-gcc mingw-w64-x86_64-ncurses make` to install necessary packages

5. `git clone https://github.com/lshprung/terminal-media-launcher` to clone the repository

6. `cd terminal-media-launcher` to enter the repository

7. `make` to compile

If you did everything correctly, compilation should succeed. Open File Explorer in the current directory (can be done by running `explorer .` in MSYS2 terminal) and double click the generated executable (should be called terminal-media-launcher.exe)

## Resolving Missing .dll Files

You may receive an error message upon trying to run the compiled executable that looks something like this:

```
The code execution cannot proceed because
libwinpthread-1.dll was not found. Reinstalling the program
may fix this problem
```

This is usually the result of the MSYS2 MinGW libraries not being in your PATH. There are generally two ways to fix this problem:

### 1. Edit PATH (Recommended)

Add the following entry to your PATH environment variable: `C:\msys64\mingw64\bin` (replace `C:\msys64` with the location that MSYS2 was installed to). This will help Windows to find the library it thinks it is missing and the program should now lauch without an error

### 2. Compile with -static flag

If you do not want to edit your PATH, you can simply compile statically. Keep in mind that this will increase the size of your executable.

1. Run `make clean`

2. Run `make CC="gcc -static"`

The resulting executable should launch without an error

---

These instructions were written with 64-bit architecture in mind. Although I have not tested on a 32-bit architecture, I suppose these instructions would also work, as long as you replace every reference to `64` with `32`
