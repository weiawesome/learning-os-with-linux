# Experiment

<div align="center"> 

[📖 中文](experiment.zh.md) | [📖 English](experiment.md)

</div>

## Program Design Experiment

```rs
// main.rs
fn main() {
    let x = 10 + 20;           // ← CPU executes addition directly
    println!("result: {}", x);   // ← Needs system call (write to terminal)
}
```

```c
// main.c
#include <stdio.h>

int main() {
    int x = 10 + 20;           // ← CPU executes addition directly
    printf("result: %d\n", x);   // ← Needs system call (write to terminal)
    return 0;
}
```

## Experiment Result (via strace)

```shell
$ strace ./main
execve("./main", ["./main"], 0x7ffd8da74670 /* 71 vars */) = 0
brk(NULL)                               = 0x57da9cf1c000
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x75c08333e000
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=69055, ...}) = 0
mmap(NULL, 69055, PROT_READ, MAP_PRIVATE, 3, 0) = 0x75c08332d000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libgcc_s.so.1", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\0\0\0\0\0\0\0\0"..., 832) = 832
fstat(3, {st_mode=S_IFREG|0644, st_size=183024, ...}) = 0
mmap(NULL, 185256, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x75c0832ff000
mmap(0x75c083303000, 147456, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x4000) = 0x75c083303000
mmap(0x75c083327000, 16384, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x28000) = 0x75c083327000
mmap(0x75c08332b000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x2b000) = 0x75c08332b000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\220\243\2\0\0\0\0\0"..., 832) = 832
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
fstat(3, {st_mode=S_IFREG|0755, st_size=2125328, ...}) = 0
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
mmap(NULL, 2170256, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x75c083000000
mmap(0x75c083028000, 1605632, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x28000) = 0x75c083028000
mmap(0x75c0831b0000, 323584, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1b0000) = 0x75c0831b0000
mmap(0x75c0831ff000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1fe000) = 0x75c0831ff000
mmap(0x75c083205000, 52624, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x75c083205000
close(3)                                = 0
mmap(NULL, 12288, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x75c0832fc000
arch_prctl(ARCH_SET_FS, 0x75c0832fc780) = 0
set_tid_address(0x75c0832fca50)         = 35155
set_robust_list(0x75c0832fca60, 24)     = 0
rseq(0x75c0832fd0a0, 0x20, 0, 0x53053053) = 0
mprotect(0x75c0831ff000, 16384, PROT_READ) = 0
mprotect(0x75c08332b000, 4096, PROT_READ) = 0
mprotect(0x57da775bf000, 16384, PROT_READ) = 0
mprotect(0x75c08337e000, 8192, PROT_READ) = 0
prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
munmap(0x75c08332d000, 69055)           = 0
poll([{fd=0, events=0}, {fd=1, events=0}, {fd=2, events=0}], 3, 0) = 0 (Timeout)
rt_sigaction(SIGPIPE, {sa_handler=SIG_IGN, sa_mask=[PIPE], sa_flags=SA_RESTORER|SA_RESTART, sa_restorer=0x75c083045330}, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=0}, 8) = 0
getrandom("\x5f\x96\xe5\x92\x04\x1c\x78\x04", 8, GRND_NONBLOCK) = 8
brk(NULL)                               = 0x57da9cf1c000
brk(0x57da9cf3d000)                     = 0x57da9cf3d000
openat(AT_FDCWD, "/proc/self/maps", O_RDONLY|O_CLOEXEC) = 3
prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
fstat(3, {st_mode=S_IFREG|0444, st_size=0, ...}) = 0
read(3, "57da7756a000-57da7757e000 r--p 0"..., 1024) = 1024
read(3, "31ff000-75c083203000 r--p 001fe0"..., 1024) = 1024
read(3, "346000 r--p 00000000 00:00 0    "..., 1024) = 913
close(3)                                = 0
sched_getaffinity(35155, 32, [0 1 2 3 4 5 6 7]) = 8
rt_sigaction(SIGSEGV, NULL, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=0}, 8) = 0
sigaltstack(NULL, {ss_sp=NULL, ss_flags=SS_DISABLE, ss_size=0}) = 0
mmap(NULL, 12288, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK, -1, 0) = 0x75c08333b000
mprotect(0x75c08333b000, 4096, PROT_NONE) = 0
sigaltstack({ss_sp=0x75c08333c000, ss_flags=0, ss_size=8192}, NULL) = 0
rt_sigaction(SIGSEGV, {sa_handler=0x57da7758e6a0, sa_mask=[], sa_flags=SA_RESTORER|SA_ONSTACK|SA_SIGINFO, sa_restorer=0x75c083045330}, NULL, 8) = 0
rt_sigaction(SIGBUS, NULL, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=0}, 8) = 0
rt_sigaction(SIGBUS, {sa_handler=0x57da7758e6a0, sa_mask=[], sa_flags=SA_RESTORER|SA_ONSTACK|SA_SIGINFO, sa_restorer=0x75c083045330}, NULL, 8) = 0
write(1, "result: 30\n", 11result: 30
)            = 11
sigaltstack({ss_sp=NULL, ss_flags=SS_DISABLE, ss_size=8192}, NULL) = 0
munmap(0x75c08333b000, 12288)           = 0
exit_group(0)                           = ?
+++ exited with 0 +++
```

We can observe some key points:

1. execve system call -> system kernel loads machine code into memory
2. write system call -> system kernel writes result to terminal (corresponds to the print instructions in the code)
3. exit_group system call -> system kernel terminates the program

We can see that everything is handled by the kernel.

## Finally, let's review the concepts again

- A high-level language program (like Rust or C) runs on the OS in a simple process:

  1. **Compilation**：Source code is compiled into machine code (program file) that the machine can understand.
  2. **Loading**：When running, the System Kernel loads the program from disk into memory.
  3. **Execution**：CPU directly executes pure computation instructions; when accessing files or output, it uses system calls (system call) to ask the Kernel to help.
  4. **Kernel Processing**：Kernel handles these system calls and is responsible for communicating with hardware, managing resources, etc.

- In summary: your program goes through compilation, loading, CPU execution, and whenever it needs hardware or system resources, it uses system calls to ask the kernel to help you.