# From Boot to Running: System Architecture Overview

<div align="center">

[📖 中文](README.zh.md) | [📖 English](README.md)

</div>

## 0. Before You Start: Migrating from Windows to Linux

See [Migrating Windows to Linux](./migrating-windows-to-linux.md) for more details.

## 1. From Boot to Running
```
1. Press the power button
   ↓
2. BIOS/UEFI initializes hardware
   - Checks CPU, memory, disk
   ↓
3. BIOS/UEFI loads Bootloader (GRUB) into memory
   - Reads from a specific position on the disk
   ↓
4. GRUB displays the menu ← where you select OS/Kernel
   ↓
5. GRUB loads the Kernel into memory
   - Reads from /boot/vmlinuz
   - Loads initramfs (temporary file system) at the same time
   ↓
6. Kernel starts running (in memory)
   - Initializes hardware
   - Sets up memory management
   - Mounts the root filesystem
   ↓
7. Kernel starts the first process: init (PID 1)
   - For modern Linux this is usually systemd
   ↓
8. systemd launches User Space
   - Starts various system services
   - Prepares login environment
   ↓
9. System is ready!
```

### Verification Process

```shell
ls /boot
```

```shell
ps -p 1 -o pid,comm,args
```

![](./assets/command-result.png)

Through the command and result, we can verify:
1. The /boot directory contains the vmlinuz file
2. PID 1 is the systemd program

### Q&A for Understanding the Process

#### Q: What is a Bootloader (GRUB)?
> A: The Bootloader is the program responsible for loading the Kernel when starting the computer.

#### Q: What content is on a bootable USB drive?
> A: Besides the pre-downloaded ISO image, a bootable USB contains the Bootloader (GRUB) and the Linux Kernel, etc.

#### Q: Does the Linux Kernel keep running all the time?
> A: Yes, the Linux Kernel keeps running until shutdown, and it resides in memory rather than on the disk.

## 2. System Architecture Overview
```
┌─────────────────────────────────────┐
│        User Applications            │
│    (browser, editor, game, etc.)    │
├─────────────────────────────────────┤
│         System Libraries            │
│       (libc, libpthread...)         │
├─────────────────────────────────────┤
│         System Call Interface       │
│      (read, write, fork, exec...)   │
╞═════════════════════════════════════╡ ← User/Kernel
│          Linux Kernel               │
│  ┌─────────────────────────────┐    │
│  │   Process Management        │    │
│  │   Memory Management         │    │
│  │   File System               │    │
│  │   Device Drivers            │    │
│  │   Network Stack             │    │
│  └─────────────────────────────┘    │
├─────────────────────────────────────┤
│         Hardware                    │
│   (CPU, RAM, Disk, Network...)      │
└─────────────────────────────────────┘
```

To be honest, during my studies, I never really touched this area (stayed in the "safe zone" of Windows for too long, haha – totally unfamiliar with the details!).

First, User Applications are the programs we use daily, e.g., browsers, editors, games, etc.
1. They're separated from the Kernel to prevent damage to the Kernel.
2. Different users (like root or a regular user) can run different applications.
3. Try the `whoami` command to see which user you are right now.

System Libraries are system libraries such as libc, libpthread, etc.
1. They are also separated from the Kernel for protection.
2. Different users can have different versions of system libraries (for example, different libc for root and users).
3. Try the `ldd` command on your program to see its linked libraries.

System Call Interface is the interface for system calls, like read, write, fork, exec, etc.
1. It's another layer of separation to protect the Kernel.
2. Different users may use different system calls in different ways.
3. Try `man 2 read` to check out the documentation for system calls used by your programs.

```
Note:
  In software development, the concept of "Interface" is very common; interestingly, it's the same principle here.
  The upper layer applications should not directly access implementations—only call the defined interface methods.

  In OS design, it's the same idea:
  Applications can only call the System Call Interface, not direct Kernel functions.
  This ensures Kernel stability and security.
```

## 3. Verification Experiment
Let's test the theory above with a simple Rust program.


- High-level language programs (like Rust or C) go through these steps before running on the OS:
  1. **Compilation**
     - Source code is compiled (by e.g., rustc or gcc) into assembly, which is then converted by an assembler to machine code.
     - The result is typically an ELF file (the common executable format on Linux).
  2. **Loading**
     - When the user runs the program (e.g., `./my_program`), the OS uses the `exec` system call to load the machine code from disk into memory (RAM).
  3. **CPU Execution**
     - The CPU fetches and executes instructions from memory, which must be in machine code it can understand.
     - Pure computation instructions are executed directly by the CPU; if the program needs to access OS resources (e.g., files, network), it uses system libraries (like glibc), which make system calls.
  4. **System Calls and the Kernel**
     - When necessary, the system library makes the corresponding system call to interact with the Linux Kernel, which handles low-level operations like file I/O, networking, etc.
     - The Kernel is also responsible for scheduling (distributing work to the CPU), managing resources, and ensuring security.
- In summary: a high-level language program is compiled, loaded into memory by the OS, executed by the CPU, and whenever it needs OS support it uses system calls, letting the Kernel handle hardware or resource access.

The full workflow looks like this:
```
┌──────────────────────────────────────────────────────┐
│  Stage 1: Compilation (Development Time)             │
│                                                      │
│  Rust source code                                    │
│       ↓ [rustc compiler]                             │
│  Assembly code                                       │
│       ↓ [assembler]                                  │
│  Machine code                                        │
│       ↓                                              │
│  Executable file stored on disk (ELF binary)         │
└──────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────┐
│  Stage 2: Loading (When Running the Program)         │
│                                                      │
│  User runs the program (e.g., ./my_program)          │
│       ↓                                              │
│  Kernel reads the machine code from disk             │
│       ↓ [exec system call]                           │
│  Loads into RAM (memory)                             │
│       ↓                                              │
│  CPU begins fetching instructions from RAM           │
└──────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────┐
│  Stage 3: Execution (Runtime)                        │
│                                                      │
│  ┌─────────────────────────────────────────┐         │
│  │  User Space (Your Program)              │         │
│  │                                         │         │
│  │  CPU executes machine code:             │         │
│  │  • Pure computation → executes directly │         │
│  │  • Needs hardware → calls system library│         │
│  │                      ↓                  │         │
│  │                 System Call             │         │
│  └─────────────────────┬───────────────────┘         │
│                        │                             │
│  ═════════════════════════════════════════════       │
│                        ↓                             │
│  ┌─────────────────────────────────────────┐         │
│  │  Kernel Space (the Kernel)              │         │
│  │                                         │         │
│  │  • Handles the system call              │         │
│  │  • Operates hardware (files/net/mem...) │         │
│  │  • Returns result                       │         │
│  └─────────────────────┬───────────────────┘         │
│                        │                             │
│                        ↓                             │
│  Program continues with the next instruction         │
└──────────────────────────────────────────────────────┘
```

See [Experiment](./experiment.md) for more details.