## Introduction

Starting in version 0.2, Terminal Media Launcher is configured via a [lua](https://www.lua.org/) script. This project assumes the configuration will adhere to lua 5.1, but is written to be as agnostic as possbile. The general format of the configuration is outlined below:

```lua
Groups = {
    {
        name = <string>,
        Launcher = <string>,
        Flags = <string>,
        Entries = {
            {
                name = <string>,
                path = <string>
            },
            ...
        }
    },
    ...
}
```

<!--
**`config.lua`** specifies settings for Terminal Media Launcher, including preferences, groups, entries, and file locations. Each line of `config` is read by terminal-media-launcher unless the line is empty or the line begins with a '#'. terminal-media-launcher can automatically generate a configuration file if no such file is found. An automatically  generated  configuration file will create groups for Music, Pictures, and Videos, and add entries to each group from the respective directory in the user's home directory. It is highly recommended that the user edit the configuration file manually.
-->

## Table of Contents

- [Creating a Group](#CreatingAGroup)
- [Adding Entries](#AddingEntries)
    - [Adding Entries using lfs module](#AddingEntriesLfs)
- [Settings](#settings)
	- [sort](#sort)
- [Example](#example)

## Creating a Group <a name="CreatingAGroup"></a>

The config must contain at least one valid group. Groups are to be inserted in the `Groups` global variable. `Groups` is an array of tables. Each group is a table containing the following keys:

- Mandatory keys:
    - `name` *string* - a name for the group
    - `Entries` *table* - an array of tables. Each table represents an entry.
- Optional keys:
    - `Launcher` *string* - the path for a program that launches the entries of this group. If not set, it assumes the entries in this group are executables (and have no launching program)
    - `Flags` *string* - flags to pass to the launching program. If not set, no flags are passed

Here is a helper function you can use in your config to add a group

```lua
local function addGroup(name, launcher, flags)
	assert(type(name) == "string")

    -- create Groups table if needed
    if Groups == nil then
        Groups = {}
    end

    local new_group = {}
    new_group.name = name
	new_group.Entries = {}
    if launcher ~= nil then
        new_group.Launcher = launcher
    end
    if flags ~= nil then
        new_group.Flags = flags
    end

    table.insert(Groups, new_group)
    return new_group
end
```

## Adding Entries <a name=AddingEntries></a>

For a group to be valid, it must contain at least one valid entry. Entries are to be inserted in the `Entries` key of a group table. `Entries` is an array of tables. Each entry is a table containing the following keys:

- Mandatory keys:
    - `name` *string* - a name for the entry
- Optional keys:
    - `path` *string* - a path to the file associated with this entry. If not set, the path is the same as the name

Here is a helper function you can use in your config to add an entry to a group

```lua
local function addEntry(parentGroup, name, path)
	assert(type(parentGroup) == "table")
	assert(type(parentGroup.Entries) == "table")
	assert(type(name) == "string")

	local new_entry = {}
	new_entry.name = name
	if path ~= nil then
		new_entry.path = path
	end

	table.insert(parentGroup.Entries, new_entry)
	return new_entry
end
```

### Adding Entries using lfs module <a name=AddingEntriesLfs></a>

It is recommended to install the [lua-filesystem](https://github.com/lunarmodules/luafilesystem) module and use it to add entries more efficiently. This example can be used to descend into a directory and add files matching a `string.match` pattern to a group.

```lua
local lfs = require "lfs"

local function addEntries(parentGroup, startDir, filePattern, recursive)
	-- recursive arg is a boolean for whether or not to descend into subdirectories (false by default)
	assert(type(parentGroup) == "table")
	assert(type(parentGroup.Entries) == "table")
	assert(type(startDir) == "string")
	assert(type(filePattern) == "string")

	for file in lfs.dir(startDir) do
		local fullFilePath = startDir .. "/" .. file
		if file ~= "." and file ~= ".." then
			-- descend into subdirectory if recursive is set to true
			if lfs.attributes(fullFilePath).mode == "directory" and recursive == true then
				addEntries(parentGroup, fullFilePath, filePattern, recursive)
			elseif lfs.attributes(fullFilePath).mode == "file" then
				if string.match(file, filePattern) then
					table.insert(parentGroup.Entries, {
						name = file,
						path = '"' .. fullFilePath .. '"'
					})
				end
			end
		end
	end
end
```

## Settings

Settings can be set via the `Settings` global variable.

```lua
Settings = {}
Settings.sort = <boolean>
```

<!--
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
-->

### sort

`sort` will sort entries of each group in alphabetical order (though not the list of groups). `sort` is true by default.

## Example

Here is an example `config.lua` which creates a Music, Pictures, and, Videos group, and adds all the files from the user's Music, Pictures, and Videos home folders to each group.

```lua
local lfs = require "lfs"

local function addGroup(name, launcher, flags)
	assert(type(name) == "string")

    -- create Groups table if needed
    if Groups == nil then
        Groups = {}
    end

    local new_group = {}
    new_group.name = name
	new_group.Entries = {}
    if launcher ~= nil then
        new_group.Launcher = launcher
    end
    if flags ~= nil then
        new_group.Flags = flags
    end

    table.insert(Groups, new_group)
    return new_group
end

local function addEntries(parentGroup, startDir, filePattern, recursive)
	-- recursive arg is a boolean for whether or not to descend into subdirectories (false by default)
	assert(type(parentGroup) == "table")
	assert(type(parentGroup.Entries) == "table")
	assert(type(startDir) == "string")
	assert(type(filePattern) == "string")

	for file in lfs.dir(startDir) do
		local fullFilePath = startDir .. "/" .. file
		if file ~= "." and file ~= ".." then
			-- descend into subdirectory if recursive is set to true
			if lfs.attributes(fullFilePath).mode == "directory" and recursive == true then
				addEntries(parentGroup, fullFilePath, filePattern, recursive)
			elseif lfs.attributes(fullFilePath).mode == "file" then
				if string.match(file, filePattern) then
					table.insert(parentGroup.Entries, {
						name = file,
						path = '"' .. fullFilePath .. '"'
					})
				end
			end
		end
	end
end

local music = addGroup("Music", "xdg-open")
addEntries(music, os.getenv("HOME") .. "/Music", ".*", true)

local pictures = addGroup("Pictures", "xdg-open")
addEntries(pictures, os.getenv("HOME") .. "/Pictures", ".*", true)

local videos = addGroup("Videos", "xdg-open")
addEntries(videos, os.getenv("HOME") .. "/Videos", ".*", true)
```
