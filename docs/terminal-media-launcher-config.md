## Introduction

**config** specifies settings for Terminal Media Launcher, including preferences, groups, entries, and file locations. Each line of `config` is read by terminal-media-launcher unless the line is empty or the line begins with a '#'. terminal-media-launcher can automatically generate a configuration file if no such file is found. An automatically  generated  configuration file will create groups for Music, Pictures, and Videos, and add entries to each group from the respective directory in the user's home directory. It is highly recommended that the user edit the configuration file manually.

## Table of Contents

- [Creating a Group](#CreatingAGroup)
	- [addGroup](#addgroup)
	- [setLauncher](#setlauncher)
	- [setLauncherRaw](#setlauncherraw)
	- [setFlags](#setflags)
- [Adding Entries](#AddingEntries)
	- [add](#add)
	- [addF](#addf)
	- [addName](#addname)
	- [addNameF](#addnamef)
	- [addR](#addr)
	- [hide](#hide)
	- [hideFile](#hidefile)
- [Settings](#settings)
	- [autoAlias](#autoalias)
	- [foldCase](#foldcase)
	- [sort](#sort)
- [Example](#example)

## Creating a Group <a name="CreatingAGroup"></a>

terminal-media-launcher will not work without any groups, so you will need to know how to create a group.

### addGroup

- **addGroup** *name*

`addGroup` will create a new group with a specified name. By default this group is empty, with zero entries, no launching application specified, and no flags specified. If there is a space in the name, it must be written in quotes (ex. "TV Shows")

### setLauncher

- **setLauncher** *group* */path/to/launcher*

`setLauncher` will set a group's launching application. If no launching application is specified for a group, terminal-media-launcher will treat each entry in that group as an executable file. If there is a space in the path to the launching application, it must be written in quotes (ex. "/usr/bin/my launcher"). *Keep in mind that the path to the launching application should be absolute*.

### setLauncherRaw

- **setLauncherRaw** *group* */path/to/launcher*

`setLauncherRaw` is identical to `setLauncher` with the exception that the launcher application specified will not be wrapped in quotes for the system call when a member of the group is launched. This can be used to specify more complex launching instructions.

### setFlags

- **setFlags** *group* *flags*

`setFlags` will set the flags to be specified for the launching application. If no launching application is specified, any specified flags are ignored. If the specified flags contain a space, they must be written in quotes.

## Adding Entries <a name=AddingEntries></a>

terminal-media-launcher will hide empty groups, so you will need to know how to add entries to a group.

### add

- **add** *path/to/file(s)* *group*

`add` will add a file to a specified group if the path exists. It can also be used to add mutiple files to a group in one line using the '\*' operator (ex. `add /home/john/Pictures/* Pictures`). If the path to the file(s) contains space(s), it must be written in quotes.

### addF

- **addF** *new-entry* *group*

`addF` will force an entry to be added to a specified group, regardless as to whether it is a valid file or not. Unlike `add`, `addF`'s argument does not need to be a valid file, but `addF` can only specify a single entry and does not support the '\*' operator. If the arg has a space in it, it must be written in quotes.

### addName

- **addName** *name* *path/to/file* *group*

`addName`, like `add`, will add an entry to a specified group if the path exists. `addName` allows for a name to be specified for this entry (by default, the name is the same as the file name). Unlike `add`, only one entry can be added per line, as `addName` does not support the '\*' operator. If either the name or file path contain a space, they must be written in quotes.

### addNameF

- **addNameF** *name* *new-entry* *group*

`addNameF` can be used in place of `addF` if you want the forced argument to have a different name displayed for the entry than is called in the system call to launch the entry. Otherwise, it is effectively the same as `addF`

### addR

- **addR** *path/to/files* *group*

`addR` will recursively add entries to a group. `addR` functions like `add`, but will also search sub-directories for matches. 

### hide

- **hide** *entry* *group*

`hide` will remove a specified entry from a specified group. The entry argument should refer to the entry's name, rather than the entry's path. This option may be useful to hide certain entries after adding entries with the '\*' operator. *At the moment, hide can only hide a single entry*.

### hideFile

- **hideFile** *path* *group*

`hideFile` has the exact same functionality as `hide`, but takes the absolute path of the entry to hide as the first argument, instead of the name.

## Settings

If any of the following settings are specified, they should be at the top of the config file.

### autoAlias

- **autoAlias** *on/off*

`autoAlias` will attempt to automatically give entries more human-readable names by:

1. Removing any characters inside parenthesis (including parenthesis)
2. Replacing '-' and '\_' with a space character
3. Replacing cases of multiple spaces in a row with only one space
4. Removing file extensions (if the file has an extension) 

`autoAlias` is turned off by default.

### foldCase

- **foldCase** *on/off*

Entering any non-traversal input in terminal-media-launcher can be used to jump to a group or entry. For instance, hitting 'f' on the keyboard will jump the cursor to the next group or entry that starts with an 'f'. *foldCase* determines whether or not this functionality is **case insensitive (on)** or **case sensitive (off)**. *foldCase* is turned on by default.

### sort

- **sort** *on/off*

`sort` will sort entries of each group in alphabetical order (though not the list of groups). Turning off `sort` is only recommended when adding one item per line to a group. `sort` is turned on by default.

## Example

```
autoAlias on

# Adding a Group of Various Applications

addGroup Applications
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
setFlags Pictures "-s f"
add /home/john/Pictures/*jpg Pictures
add /home/john/Pictures/*png Pictures
addR "/mnt/External_Drive/Johns Photos/*" Pictures
addName "My Desktop Background" "/mnt/External_Drive/desktop wallpaper.jpg" Pictures
```
