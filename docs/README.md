## Introduction

**Terminal Media Launcher** is a command line utility to help streamline launching applications and other media. The goal of this program is to provide a fast, minimal, command line frontend with a Unix-like approach to setup and configuration. The program looks for a configuration file listing different groups of media and creates an ncurses menu from which to select from. If a configuration file is found, the program will draw two columns: one for groups, and one for entries. At the bottom of the window, a preview of the command execution is displayed.

## Compiling and Running

For Windows instructions, see [here](windows_compile_instructions.md)

terminal-media-launcher can be compiled on any system with make, gcc, and the ncurses library (libncurses-dev) installed. It can be compiled and installed on any Linux distribution, and can also be compiled and run on Windows 10. To compile and run terminal-media-launcher:

1. Clone the repository
2. Run the commands below in the directory the repository was cloned into. This will build the binary `src/terminal-media-launcher`

```
$ ./configure
$ make
```

3. To run the program:

```
$ src/terminal-media-launcher
```

Note that terminal-media-launcher will not run until you have created a configuration file.

### Installation

terminal-media-launcher can be installed on Linux by running:

```
sudo make install
```

This will install `terminal-media-launcher` to `/usr/local/bin/terminal-media-launcher` as well as man pages `terminal-media-launcher` and `terminal-media-launcher-config`.
To uninstall these files:

```
sudo make uninstall
```

### Create Desktop Entry

If terminal-media-launcher is installed (see above), a Desktop Entry can be created on Linux by running:

```
sudo make desktop-entry
```

This will create a desktop entry at `/usr/local/share/applications/terminal-media-launcher.desktop`.

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
If you installed terminal-media-launcher, you can also consult **terminal-media-launcher**(1) and **terminal-media-launcher-config**(5).
