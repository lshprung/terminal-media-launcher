![logo](../data/terminal-media-launcher.svg)

## Introduction

**Terminal Media Launcher** is a command line utility to help streamline launching applications and other media. The goal of this program is to provide a fast, minimal, command line frontend with a Unix-like approach to setup and configuration. The program looks for a configuration file listing different groups of media and creates an ncurses menu from which to select from. If a configuration file is found, the program will draw two columns: one for groups, and one for entries. At the bottom of the window, a preview of the command execution is displayed.

## Compiling and Running

For Windows instructions, see [here](windows_compile_instructions.md)

terminal-media-launcher can be compiled on any system with make, gcc, and the ncurses development library installed. It can be compiled and installed on any Linux distribution, and can also be compiled and run on Microsoft Windows. To compile and run terminal-media-launcher:

1. Install dependencies. 

On Debian and Debian-based systems:
```
# apt install gcc libncurses-dev make
```

On RHEL and RHEL-based systems:
```
# dnf install gcc ncurses-devel
```

2. Download the latest `.tar.gz` release
	- The source acquired from the source repository can also be compiled, but requires additional dependencies to build (`autoconf`, `automake`, `autoconf-archive`)
3. Run the commands below in the directory the repository was cloned into. This will build the binary `src/terminal-media-launcher`

```
$ ./configure
$ make
```

4. To run the program:

```
$ src/terminal-media-launcher
```

Note that terminal-media-launcher will not run until you have created a configuration file.

### Installation

terminal-media-launcher can be installed by running:

```
make install
```

To uninstall these files:

```
make uninstall
```

By default, the program and related files are installed with prefix `/usr/local`, but this prefix can be changed with `./configure --prefix=PREFIX` flag (i.e., `./configure --prefix=$HOME/.local`

### Create Desktop Entry

Installation of a desktop entry can be enabled by passing a flag to the configure script:

```
./configure --enable-desktop-entry
```

Setting this flag will install a desktop entry alongside the program when following the installation instructions above

## Screenshots

![screenshot 1](screenshot1.png)
![screenshot 2](screenshot2.png)

## Configuration File

By default, terminal-media-launcher searches in the following order for a configuration file:

### Linux

1. `$HOME/.config/terminal-media-launcher/config`
2. `$HOME/.terminal-media-launcher/config`

### Windows

1. `%APPDATA%\terminal-media-launcher\config`

A different configuration file location can also be specified with the `-c` flag:

```
terminal-media-launcher -c /path/to/config
```

For Documentation of the configuration file, see [terminal-media-launcher-config](terminal-media-launcher-config.md).
For a help message and list of flags, `terminal-media-launcher --help`.
You can also consult the man pages **terminal-media-launcher**(1) and **terminal-media-launcher-config**(5)
