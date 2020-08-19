## Introduction

**Terminal Media Launcher** is a command line utility to help streamline launching applications and other media. The goal of tml is to provide a fast, minimal, command line frontend with a Unix-like approach to setup and configuration. The program looks for a config file listing different groups of media and creates an ncurses menu from which to select from.

## Compiling and Running

tml can be compiled on any system with gcc and the ncurses library installed. It has been tested to work on Ubuntu and Windows 10. To compile and run tml:

1. Clone the repository
2. Run `make` in the directory the repository was cloned into. This will create a file called `tml`
3. `./tml` to run the program

Note that tml will not run until you have created a config file.

## Config File

By default, tml searches in the following order for a config file:

1. `$HOME/.config/tml/config`
2. `$HOME/.tml/config`
3. `config` (in the current directory)

A different config file location can also be specified with the `-c` flag:

```
tml -c /path/to/config
```

For Documentation of the config file, see [tml-config](tml-config.md)
