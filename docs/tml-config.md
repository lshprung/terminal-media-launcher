## Introduction

**config** specifies settings for Terminal Media Launcher, including preferences, groups, entries, and file locations. Each line of `config` is read by tml unless the line is empty or the line begins with a '#'. tml will not run if no config file exists, so you will need to know how to write one

## Table of Contents

- [Creating a Group](#CreatingAGroup)
	- [addGroup](#addGroup)
	- [setLauncher](#setLauncher)
	- [setFlags](#setFlags)
- [Adding Entries](#AddingEntries)
	- [add](#add)
	- [addF](#addF)
	- [addName](#addName)
	- [addNameF](#addNameF)
	- [addR](#addR)
	- [hide](#hide)
- [Settings](#Settings)
	- [autoAlias](#autoAlias)
	- [compMode](#compMode)
	- [foldCase](#foldCase)
	- [sort](#sort)
- [Example](#Example)

## Creating a Group <a name="CreatingAGroup"></a>

tml will not work without any groups, so you will need to know how to create a group.

### addGroup

- **addGroup** *name*

`addGroup` will create a new group with a specified name. By default this group is empty, with zero entries, no launching application specified, and no flags specified. If their is a space in the name, it must be written in quotes (ex. "TV Shows")

### setLauncher

- **setLauncher** *group* */path/to/launcher*

`setLauncher` will set a group's launching application. If no launching application is specified for a group, tml will treat each entry in that group as an executable file. If their is a space in the path to the launching application, it must be written in quotes (ex. "/usr/bin/my launcher"). *Keep in mind that the path to the launching application should be absolute*.

### setFlags

- **setFlags** *group* *flags*

`setFlags` will set the flags to be specified for the launching application. If no launching application is specified, any specified flags are ignored. If the specified flags contain a space, they must be written in quotes.

## Adding Entries <a name=AddingEntries></a>

tml will hide empty groups, so you will need to know how to add entries to a group.

### add

- **add** *path/to/file(s)* *group*

`add` will add a file to a specified group if the path exists. It can also be used to add mutiple files to a group in one line using the '\*' operator (ex. `add /home/john/Pictures/* Pictures`). If the path to the file(s) contains space(s), it must be written in quotes.

### addF

- **addF** *arg* *group*

`addF` will force an entry to be added to a specified group, regardless as to whether it is a valid file or not. Unlike `add`, `addF`'s argument does not need to be a valid file, but `addF` can only specify a single entry and does not support the '\*' operator. If the arg has a space in it, it must be written in quotes.

### addName

- **addName** *name* *path/to/file* *group*

`addName`, like `add`, will add an entry to a specified group if the path exists. `addName` allows for a name to be specified for this entry (by default, the name is the same as the file name). Unlike `add`, only one entry can be added per line, as `addName` does not support the '\*' operator. If either the name or file path contain a space, they must be written in quotes.

### addNameF

- **addNameF** *name* *arg* *group*

`addNameF` can be used in place of `addF` if you want the forced argument to have a different name displayed for the entry than is called in the system call to launch the entry. Otherwise, it is effectively the same as `addF`

### addR

- **addR** *path/to/files* *group*

`addR` will recursively add entries to a group. `addR` functions like `add`, but will also search sub-directories for matches. 

### hide

- **hide** *entry* *group*

`hide` will remove a specified entry from a specified group. The entry argument should refer to the entry's name, rather than the entry's path. This option may be useful to hide certain files after adding entries with the '\*' operator. *At the moment, hide can only hide a single entry*.

## Settings

If any of the following settings are specified, they should be at the top of the config file.

### autoAlias

- **autoAlias** *on/off*

`autoAlias` will attempt to automatically give entries more human-readable names based on their filename by:

1. Removing any characters inside parenthesis (including parenthesis)
2. Replacing '-' and '\_' with a space character
3. Replacing cases of multiple spaces in a row with only one space
4. Removing file extensions (if the file has an extension) 

`autoAlias` is turned off by default.

### compMode

- **compMode WSL**

At the moment, `compMode` can be turned on for using tml in Windows Subsystem for Linux. By default, `compMode` is turned off. *compMode may be removed in the future due to redundancy, as tml works natively in Windows*.

### foldCase

- **foldCase** *on/off*

Entering any non-traversal input in tml can be used to jump to a group or entry. For instance, hitting 'f' on the keyboard will jump the cursor to the next group or entry that starts with an 'f'. *foldCase* determines whether or not this functionality is **case insensitive (on)** or **case sensitive (off)**. *foldCase* is turned on by default.

### sort

- **sort** *on/off*

`sort` will sort entries of each group in alphabetical order (though not the list of groups). Turning off `sort` is only recommended when adding one item per line to a group. `sort` is turned on by default.

## Example

```
autoAlias on

# Adding a Group of Various Applications

addGroup Applcations
addName GIMP /usr/bin/gimp Applications
addName Chromium /usr/bin/chromium-browser Applications
addName Thunderbird /usr/bin/thunderbird Applications

# Adding a Videos Group that contains mp4 files

addGroup Videos
setLauncher Videos /usr/bin/vlc
add /home/john/Videos/*mp4 Videos

# Adding a Pictures Group that contains only jpg and png files as well as all files from an external drive and a single desktop wallpaper

addGroup Pictures
setLauncher Pictures /usr/bin/sxiv
setFlags "-s f"
add /home/john/Pictures/*jpg Pictures
add /home/john/Pictures/*png Pictures
addR "/mnt/External_Drive/Johns Photos/*" Pictures
addName "My Desktop Background" "/mnt/External_Drive/desktop wallpaper.jpg" Pictures
```
