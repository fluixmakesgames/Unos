# Unos (v 0.1)
Kernel for the Arduino Uno.
## Features
### Kernel:
- Up to 8 tasks with names of 8 characters
- Task Spawning
- Task Killing
- Ticks that go up each round robin

### Shell:
#### Commands
- help (lists commands)
- uptime (prints uptime in ticks)
- fmem (prints free memory)
- taskmon (prints the tasks with the TaskMonitor() function)
- echo (prints users input)
- kill (kills specified task)

## Examples
(">>" is used to indicate the users input)

### Boot, and uptime.
```
[X] Serial began on 9600
[X] Shell task spawned
Unos kernel v 0.1
(2026)
>> uptime
500241
```

### Hello, world!
```
>> echo Hello, world!
Hello, world!
```
