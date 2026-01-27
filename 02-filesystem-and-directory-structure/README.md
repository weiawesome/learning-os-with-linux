# Filesystem and Directory Structure

<div align="center">

[📖 中文](README.zh.md) | [📖 English](README.md)

</div>

## Continuing from Chapter 1: Filesystem Mounting After Boot

In [Chapter 1](../01-from-boot-to-run-overview/README.md), we mentioned that the Kernel "mounts the root filesystem" after startup. In this chapter, we'll dive deeper into this process and Linux's directory structure.

```
Kernel starts executing (in memory)
  - Initialize hardware
  - Set up memory management
  - Mount root filesystem      ← This chapter's focus
  ↓
System ready!
```

### What is "Mounting"?

Mounting is like "connecting" a storage device to the system so you can access the files inside.

Real-life analogy:
- A hard drive is like a safe full of documents
- Mounting is like opening the safe and placing it on your desk
- The mount point is the location on your desk where you put the safe

### Verify Current Mounts

```bash
# View root directory mount
mount | grep "on / "

# View all mount points
df -h
```

### Transition from initramfs to Real Root Filesystem

Mount sequence during boot:

```
1. GRUB loads initramfs (temporary filesystem)
   - A compressed mini Linux in memory
   - Contains essential drivers
   ↓
2. initramfs loads disk drivers
   - Now we can read the actual disk
   ↓
3. Switch to real root filesystem
   - From initramfs to the / on disk
   - This is called "pivot_root" or "switch_root"
```

Why do we need initramfs?
- The Kernel doesn't include all disk drivers
- initramfs provides a temporary environment to load necessary drivers
- Only after loading drivers can we access the real root filesystem

## Linux Filesystem Hierarchy Standard (FHS)

FHS (Filesystem Hierarchy Standard) defines the standard Linux directory structure.

```
/                           ← Root directory, the starting point
├── bin/      → Basic commands (ls, cp, cat...)
├── boot/     → Boot files (vmlinuz, initramfs)  ← Mentioned in Ch.1
├── dev/      → Device files (disks, terminals...)
├── etc/      → System config files (configuration central)
├── home/     → User home directories (/home/your_name)
├── lib/      → System libraries (shared code)
├── media/    → Removable media (USB, CD-ROM)
├── mnt/      → Temporary mount points
├── opt/      → Third-party software
├── proc/     → Process info (virtual filesystem)      ← Important!
├── root/     → Root user's home directory
├── run/      → Runtime information
├── sbin/     → System admin commands (requires root)
├── sys/      → System info (virtual filesystem)       ← Important!
├── tmp/      → Temporary files (cleared on reboot)
├── usr/      → User programs (most software here)
│   ├── bin/  → User commands
│   ├── lib/  → Program libraries
│   └── share/→ Shared data
└── var/      → Variable data (logs, cache, mail)
    └── log/  → System logs
```

### Directory Memory Tips

| Directory | Memory Trick | Purpose |
|-----------|--------------|---------|
| `/bin` | **bin**ary | Basic executables |
| `/etc` | **et c**etera | Config files |
| `/home` | home | User data |
| `/tmp` | **t**e**mp**orary | Temp files |
| `/var` | **var**iable | Variable data |
| `/usr` | **U**nix **S**ystem **R**esources | User programs |

### Verify Directory Structure

```bash
# List root directory
ls -la /

# Display as tree (first level only)
tree -L 1 /
```

## Virtual Filesystems: /proc, /sys, /dev

These three directories are special: they're not real files on disk, but "windows" provided by the Kernel.

```
┌─────────────────────────────────────────────────────┐
│                    User Space                        │
│                                                      │
│   Want to know CPU info?                             │
│        │                                             │
│        ↓                                             │
│   cat /proc/cpuinfo                                  │
│        │                                             │
└────────┼────────────────────────────────────────────┘
         │
═════════╪══════════════════════════════════════════════
         │ (Looks like reading a file, actually asking Kernel)
         ↓
┌─────────────────────────────────────────────────────┐
│                   Kernel Space                       │
│                                                      │
│   Received request to read /proc/cpuinfo            │
│        │                                             │
│        ↓                                             │
│   Kernel generates CPU info on-the-fly and returns  │
│   (This data is NOT stored on disk)                 │
│                                                      │
└─────────────────────────────────────────────────────┘
```

### Quick Overview

| Directory | Purpose | Example |
|-----------|---------|---------|
| `/proc` | Process & Kernel info | `cat /proc/cpuinfo` |
| `/sys` | Hardware & drivers | `ls /sys/class/net` |
| `/dev` | Device files | `ls -la /dev/sda` |

### Real-life Analogy: Car Dashboard

Imagine your computer is a car:
- **Files on disk** = Things in the car (luggage, documents)
- **/proc** = Dashboard showing real-time info (speed, fuel, RPM)
- **/sys** = Control panel (adjustable settings)
- **/dev** = Steering wheel, accelerator, brakes (interfaces to hardware)

### Basic Verification Commands

```bash
# /proc - View system information
cat /proc/version          # Kernel version
cat /proc/cpuinfo | head   # CPU info
cat /proc/meminfo | head   # Memory info

# /proc/[PID] - View process information
ls /proc/1/                # Info about PID 1
ls -la /proc/1/exe         # Executable location

# /sys - View hardware information
ls /sys/class              # Hardware classes
ls /sys/class/net          # Network interfaces

# /dev - View devices
ls -la /dev/sda            # First hard drive
ls -la /dev/null           # Black hole device
```

> 💡 **Deep Dive**: For detailed information about virtual filesystems, including how to control hardware, monitor systems, and write programs that access them, see: [Virtual Filesystem Deep Dive](./virtual-filesystem.md)

## File Types

In Linux, "everything is a file". Use `ls -l` to see different types:

```bash
ls -la /
```

The first character indicates the file type:

```
drwxr-xr-x   - bin/        d = directory
-rw-r--r--   - some_file   - = regular file
lrwxrwxrwx   - lib -> ...  l = symbolic link
brw-rw----   - /dev/sda    b = block device
crw-rw-rw-   - /dev/null   c = character device
```

### File Types Explained

| Symbol | Type | Description | Example |
|--------|------|-------------|---------|
| `-` | Regular file | Text, programs, images | `/etc/passwd` |
| `d` | Directory | Folder | `/home` |
| `l` | Symbolic link | Shortcut/soft link | `/lib` → `/usr/lib` |
| `b` | Block device | Hard drive, USB | `/dev/sda` |
| `c` | Character device | Terminal, keyboard | `/dev/tty` |
| `p` | Named pipe | Inter-process communication | FIFO |
| `s` | Socket | Network communication | `/run/docker.sock` |

### Symbolic Link Example

```bash
# Create a symbolic link
ln -s /usr/bin/python3 ~/my_python

# View where link points
ls -la ~/my_python

# Use the link
~/my_python --version

# Remove link (doesn't affect original file)
rm ~/my_python
```

## Command Summary

```bash
# === View mounts ===
mount | head -10
df -h
findmnt /

# === Explore directories ===
ls -la /
tree -L 1 /

# === Virtual filesystems (basics) ===
cat /proc/version
cat /proc/cpuinfo | head -10
cat /proc/meminfo | head -5
ls /sys/class
ls -la /dev | head -15

# For more virtual filesystem commands, see: virtual-filesystem.md

# === File types ===
file /bin/ls           # Check file type
file /dev/null
file /proc/cpuinfo
```

## Connection to Chapter 1

| Mentioned in Ch.1 | Expanded in Ch.2 |
|-------------------|------------------|
| Kernel mounts root filesystem | Detailed mount process and initramfs |
| /boot has vmlinuz | Complete FHS directory structure |
| Kernel runs in memory | /proc shows real-time Kernel info |

## Preparing for Chapter 3

Content introduced in this chapter will be used in Chapter 3 "Process":

- `/proc/[pid]` - Detailed info for each process
- `/bin`, `/usr/bin` - Where executables are stored
- `/dev/tty` - Process terminals

Next chapter we'll dive deep into "what is a process" and how to manage them!

## Further Reading

### In This Chapter
- [Virtual Filesystem Deep Dive](./virtual-filesystem.md) - Detailed exploration of /proc, /sys, and /dev

### External Resources
- [Filesystem Hierarchy Standard](https://refspecs.linuxfoundation.org/FHS_3.0/fhs-3.0.html)
- [The /proc filesystem](https://www.kernel.org/doc/html/latest/filesystems/proc.html)

## Experiments

See: [Experiments](./experiment.md)
