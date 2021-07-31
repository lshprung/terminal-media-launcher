# Tasklist

## FIXME

- If giving invalid config path for -c|--cfg\_path flag, program crashes
	- Should give a formal error message
- On Windows, running without alias option turned on in cfg causes instability

### Completed

---

## TODO

- Make F5 reload the program (including rereading the cfg file)
- Add cfg option "addRaw" and "addNameRaw" to add an entry that does not get quoted in the syscall
	- This can lead to greater possibilites, such as custom launchers per entry and passing suffix flags
	- These options would deprecate "addNameF" and similar options

### Completed
