# sentinel

A simple tool that monitors the execution of a program and
once that program is done, executes the specified program to it.

sentinel makes use of the `ptrace()` Linux system call.

## Use
shell 1
```
$ sen "meson setup _build"
PID: 12305
The Meson build system
Version: 1.3.2
...
```

shell 2

Pass the pid of the previous process to this `sen` instance
```
$ sen -w 12305 "meson compile _build"
PID: 12309
...
```

The above example doesn't much explain the overall significance since build
systems like `meson` don't require different envs. This program
could be very helpful in build setups requiring different environments.

## Build
```
make && sudo make install
```

### Permissions

By default, the kernel only lets a process to ptrace() it's
child processes. To attach to a non-related process, You either
need to be a superuser or you could set `/proc` value by using
the following command:

```
echo 0 > /proc/sys/kernel/yama/ptrace_scope
```
Note: This change is not persistent across reboot.

To make this change permanent, which I wouldn't recommend, modify the
variable's value in `/etc/sysctl.d/10-ptrace.conf` and set it to `0`.
