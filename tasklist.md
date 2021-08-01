# Tasklist

## FIXME

- **On Windows, running without alias option turned on in cfg causes instability**

### Completed

- If giving invalid config path for -c|--cfg\_path flag, program crashes

---

## TODO

- **Make F5 reload the program (including rereading the cfg file)**
- **Add cfg option "addRaw" and "addNameRaw" to add an entry that does not get quoted in the syscall**
	- This can lead to greater possibilites, such as custom launchers per entry and passing suffix flags
	- These options would deprecate "addNameF" and similar options

### Completed

- Create a task list to keep track of ideas and bugs - Added in commit `c21fbd4582ef016c4b195bfa7db8727914befb15`
