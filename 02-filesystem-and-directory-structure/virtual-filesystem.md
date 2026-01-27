# Virtual Filesystem Deep Dive

<div align="center">

[📖 中文](virtual-filesystem.zh.md) | [📖 English](virtual-filesystem.md)

</div>

## What is a Virtual Filesystem?

A Virtual Filesystem is a special type of filesystem whose contents **do not exist on disk** but are **generated on-the-fly** by the Kernel in memory.

### Why Do We Need Virtual Filesystems?

Linux philosophy: "Everything is a file", including:
- Hardware devices
- Process information
- Kernel state
- System configuration

Benefits of the file interface:
- Unified operations (read, write, open, close)
- Standard tools work (cat, echo, grep)
- No special APIs needed for programs

```
┌─────────────────────────────────────────────────────┐
│                     User Space                      │
│                                                     │
│   Applications use standard file operations         │
│   read(), write(), open(), close()                  │
│        │                                            │
│        ↓                                            │
│   cat /proc/cpuinfo                                 │
│   echo 1 > /sys/class/leds/led0/brightness          │
│   dd if=/dev/urandom of=random.bin                  │
│        │                                            │
└────────┼────────────────────────────────────────────┘
         │
═════════╪══════════════════════════════════════════════
         │ (System calls: looks like file I/O)
         ↓
┌─────────────────────────────────────────────────────┐
│                   Kernel Space                      │
│                                                     │
│   VFS (Virtual Filesystem Switch) Layer             │
│        │                                            │
│        ├──→ /proc  → procfs → Generate process info │
│        ├──→ /sys   → sysfs  → Generate system info  │
│        └──→ /dev   → devfs  → Hardware devices      │
│                                                     │
│   Note: VFS also handles real filesystems (ext4, xfs)│
│         but this doc focuses on virtual filesystems  │
│                                                     │
└─────────────────────────────────────────────────────┘
```

## /proc - Process and Kernel Information

`/proc` stands for **proc**ess **f**ile**s**ystem, providing:
- Information about all running processes
- Kernel runtime state
- System hardware information

### Structure of /proc

```bash
ls -la /proc/
```

You'll see:
- **Numeric directories** (`1`, `1234`, `5678`...): Each number represents a process PID
- **Regular files** (`cpuinfo`, `meminfo`, `version`...): System information

### System-Level Information

```bash
# === CPU Information ===
cat /proc/cpuinfo
# Shows: CPU model, cores, frequency, cache size

# === Memory Information ===
cat /proc/meminfo
# Shows: Total memory, available memory, swap usage

# === Kernel Version ===
cat /proc/version
# Shows: Linux version, GCC version, build time

# === System Uptime ===
cat /proc/uptime
# Format: [total uptime seconds] [idle time seconds]

# === Loaded Modules ===
cat /proc/modules
# Lists all loaded kernel modules

# === Filesystem Types ===
cat /proc/filesystems
# Lists filesystems supported by kernel

# === Partition Information ===
cat /proc/partitions
# Lists all disk partitions

# === Network Statistics ===
cat /proc/net/dev
# Shows network interface transmission stats
```

### Process-Level Information: /proc/[PID]

Each running process has a `/proc/[PID]` directory:

```bash
# List contents of PID 1
ls -la /proc/1/

# You'll see:
# cmdline  - Command line used to start the process
# environ  - Environment variables
# exe      - Symbolic link to executable
# fd/      - Open file descriptors
# maps     - Memory mappings
# status   - Process status summary
# stat     - Detailed statistics
# cwd      - Symbolic link to current working directory
```

#### Practical Examples

```bash
# === View Process Executable Location ===
ls -la /proc/1/exe
# Example: /proc/1/exe -> /usr/lib/systemd/systemd

# === View Process Command Line ===
cat /proc/1/cmdline | tr '\0' ' '
# Note: cmdline separates args with null bytes, tr converts to spaces

# === View Process Environment Variables ===
cat /proc/1/environ | tr '\0' '\n'

# === View Process Open Files ===
ls -la /proc/1/fd/
# 0 -> stdin
# 1 -> stdout
# 2 -> stderr
# 3, 4, 5... -> other open files

# === View Process Memory Mappings ===
cat /proc/1/maps
# Shows memory locations of code, data, and shared libraries

# === View Process Status ===
cat /proc/1/status
# Shows: Name, State, PID, PPID, Threads, memory usage...
```

### /proc/self - Current Process

`/proc/self` is a special symbolic link that always points to the **current process**:

```bash
# See where /proc/self points
ls -la /proc/self
# Example: /proc/self -> 12345

# PID changes with each execution
echo $$           # Show current shell PID
ls -la /proc/self # Points to the same PID
```

### Modifying Kernel Parameters via /proc (/proc/sys)

The `/proc/sys` directory allows you to **read and modify** kernel parameters:

```bash
# === Network Parameters ===
# Check if IP forwarding is enabled
cat /proc/sys/net/ipv4/ip_forward

# Enable IP forwarding (requires root)
echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward

# === Kernel Parameters ===
# View hostname
cat /proc/sys/kernel/hostname

# View maximum PID allowed
cat /proc/sys/kernel/pid_max

# === Virtual Memory Parameters ===
# View swap tendency (0-100)
cat /proc/sys/vm/swappiness
```

Note: Changes via `/proc/sys` are **lost on reboot**. For permanent changes, edit `/etc/sysctl.conf`.

### Real-World Example: Analyzing a Running Process

Let's analyze an actual process (e.g., your terminal):

```bash
# 1. Find your terminal's PID
echo $$  # Or use: ps aux | grep bash

# 2. Assume PID is 12345, view detailed info
PID=12345

# Executable
ls -la /proc/$PID/exe

# Working directory
ls -la /proc/$PID/cwd

# Open files
ls -la /proc/$PID/fd/

# Memory usage
cat /proc/$PID/status | grep -E "VmSize|VmRSS"
```

## /sys - System and Hardware Information

`/sys` stands for **sys**tem **f**ile**s**ystem, providing:
- Hardware device information
- Kernel object model
- Device driver interfaces
- Hardware control and configuration

### Structure of /sys

```
/sys/
├── block/        → Block devices (hard drives, SSDs)
├── bus/          → Bus types (USB, PCI, I2C...)
├── class/        → Device classes (network, input, leds...)
├── devices/      → Tree structure of all devices
├── firmware/     → Firmware interfaces
├── fs/           → Filesystem information
├── kernel/       → Kernel internals
├── module/       → Loaded modules
└── power/        → Power management
```

### Common Usage Examples

```bash
# === Network Devices ===
# List all network interfaces
ls /sys/class/net/
# Example: eth0, wlan0, lo

# View MAC address of network interface
cat /sys/class/net/eth0/address

# View interface state (up/down)
cat /sys/class/net/eth0/operstate

# View network speed
cat /sys/class/net/eth0/speed

# === CPU Information ===
# List all CPUs
ls /sys/devices/system/cpu/
# Example: cpu0, cpu1, cpu2...

# View CPU frequency
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq

# View available CPU frequencies
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies

# === Block Devices (Hard Drives) ===
# List all block devices
ls /sys/block/
# Example: sda, sdb, nvme0n1

# View disk size (in 512-byte sectors)
cat /sys/block/sda/size

# View disk model
cat /sys/block/sda/device/model

# === Backlight Control ===
# View screen brightness (laptops)
cat /sys/class/backlight/*/brightness
cat /sys/class/backlight/*/max_brightness

# Adjust brightness (requires root)
echo 500 | sudo tee /sys/class/backlight/*/brightness

# === LED Control ===
# List system LEDs
ls /sys/class/leds/

# Control LED (requires root)
echo 1 | sudo tee /sys/class/leds/input3::capslock/brightness
echo 0 | sudo tee /sys/class/leds/input3::capslock/brightness
```

### Differences Between /sys and /proc

| Feature | /proc | /sys |
|---------|-------|------|
| Primary Purpose | Process info, Kernel state | Hardware devices, drivers |
| Content Format | Free-form, often multi-line | Strict, usually one value per file |
| Writability | Partially writable (/proc/sys) | Many files writable |
| Introduction | Linux 1.0 (1994) | Linux 2.6 (2003) |
| Design Philosophy | Provide process information | Unified device model |

## /dev - Device Files

`/dev` stands for **dev**ice, containing special files for all hardware devices.

### Nature of Device Files

In Linux, hardware devices are abstracted as files, using standard file operations:

```c
// Open device
int fd = open("/dev/sda", O_RDONLY);

// Read from disk
read(fd, buffer, size);

// Close device
close(fd);
```

### Device Types

```bash
ls -la /dev/ | head -20
```

Look at the first character:
- `b` = **Block Device**
- `c` = **Character Device**

#### Block Devices

**Characteristics**: Read/write in fixed-size blocks (usually 512 bytes or 4KB)

```bash
# Disks and partitions
ls -la /dev/sd*
# sda      - First SATA/SCSI disk
# sda1     - First partition
# sda2     - Second partition
# sdb      - Second disk

# NVMe SSDs
ls -la /dev/nvme*
# nvme0n1     - First NVMe SSD
# nvme0n1p1   - First partition

# Loop devices
ls -la /dev/loop*
# Used for mounting ISO images
```

#### Character Devices

**Characteristics**: Read/write as character (byte) streams

```bash
# === Special Devices ===
ls -la /dev/null     # Black hole: discards all input
ls -la /dev/zero     # Infinite stream of zeros
ls -la /dev/random   # Random number generator (blocking)
ls -la /dev/urandom  # Random number generator (non-blocking)
ls -la /dev/full     # Always reports "disk full"

# === Terminal Devices ===
ls -la /dev/tty*     # Terminals
ls -la /dev/pts/*    # Pseudo-terminals (SSH, GUI terminals)

# === Input Devices ===
ls -la /dev/input/*  # Keyboard, mouse, touchpad
```

### Common Device Uses

#### /dev/null - Data Black Hole

```bash
# Discard stderr
command 2>/dev/null

# Discard all output
command &>/dev/null

# Empty file contents
cat /dev/null > file.txt

# Test write performance (no actual disk write)
dd if=/dev/zero of=/dev/null bs=1M count=1000
```

#### /dev/zero - Generate Zeros

```bash
# Create 1GB empty file
dd if=/dev/zero of=1GB.img bs=1M count=1024

# Overwrite disk (destroy data)
sudo dd if=/dev/zero of=/dev/sdb bs=1M
```

#### /dev/random and /dev/urandom - Random Numbers

```bash
# Generate 32 bytes of random data
head -c 32 /dev/urandom | base64

# Generate random password
tr -dc A-Za-z0-9 </dev/urandom | head -c 16

# Generate random file
dd if=/dev/urandom of=random.bin bs=1M count=10
```

#### /dev/tty - Terminal

```bash
# See current terminal
tty
# Example: /dev/pts/0

# Write directly to your terminal
echo "Hello!" > /dev/tty

# Send message from another terminal (requires permission)
echo "Hi there!" > /dev/pts/1
```

### Device Major and Minor Numbers

```bash
ls -la /dev/sda*
```

Output similar to:
```
brw-rw---- 1 root disk 8, 0 Jan 24 10:00 /dev/sda
brw-rw---- 1 root disk 8, 1 Jan 24 10:00 /dev/sda1
brw-rw---- 1 root disk 8, 2 Jan 24 10:00 /dev/sda2
                       ↑  ↑
                    Major Minor
```

- **Major Number**: Identifies device driver (e.g., 8 = SCSI disk)
- **Minor Number**: Identifies specific device (e.g., 0 = sda, 1 = sda1)

### udev - Dynamic Device Management

The `/dev` directory is managed by the `udev` service:

```bash
# View udev rules
ls /etc/udev/rules.d/
ls /lib/udev/rules.d/

# Trigger udev events
sudo udevadm trigger

# Monitor udev events (shows activity when inserting USB)
sudo udevadm monitor
```

When you insert a USB drive:
1. Kernel detects new device
2. Sends event to udev
3. udev creates `/dev/sdb` (or another name)
4. udev may auto-mount to `/media/`

## Real-World Examples

### Example 1: Monitor CPU Temperature

```bash
# View temperature sensors
cat /sys/class/thermal/thermal_zone*/temp

# Convert to Celsius (many systems use millidegrees)
awk '{print $1/1000 "°C"}' /sys/class/thermal/thermal_zone0/temp

# Continuous monitoring
watch -n 1 'cat /sys/class/thermal/thermal_zone0/temp | awk "{print \$1/1000 \"°C\"}"'
```

### Example 2: Analyze Memory Usage

```bash
# View overview
free -h

# Detailed information
cat /proc/meminfo

# Find most memory-consuming processes
ps aux --sort=-%mem | head -10

# View detailed memory of specific process
cat /proc/[PID]/status | grep -E "Vm|Rss"
```

### Example 3: Network Interface Information

```bash
# List all network interfaces
ip link show
# Or
ls /sys/class/net/

# View interface statistics
cat /proc/net/dev

# View detailed info for specific interface
IFACE=eth0
echo "MAC: $(cat /sys/class/net/$IFACE/address)"
echo "MTU: $(cat /sys/class/net/$IFACE/mtu)"
echo "State: $(cat /sys/class/net/$IFACE/operstate)"
```

### Example 4: Explore USB Devices

```bash
# List USB devices
lsusb

# Detailed information
lsusb -v

# View via /sys
ls /sys/bus/usb/devices/

# View specific USB device info
cat /sys/bus/usb/devices/1-1/manufacturer
cat /sys/bus/usb/devices/1-1/product
```

## Code Examples

Want to write programs to explore virtual filesystems?

See [Experiment Document](./experiment.md) "Experiment 1: Deep Dive into Virtual Filesystems", which includes:
- Complete C implementation
- Complete Rust implementation
- Detailed code explanations
- Compilation and execution instructions

## Summary

### Comparison of Three Virtual Filesystems

| Feature | /proc | /sys | /dev |
|---------|-------|------|------|
| **Primary Purpose** | Process & Kernel info | Hardware & drivers | Device access interface |
| **Content Source** | Kernel-generated on-the-fly | Kernel device model | udev dynamically created |
| **Writable** | Partially (/proc/sys) | Many writable | Read/write (actual devices) |
| **Data Format** | Human-readable, multi-line | One value per file | Binary or character stream |
| **Typical Operation** | Read information | Read & control hardware | I/O operations |

### Key Concepts

1. **Virtual ≠ Unreal**: This information is real, just not stored on disk
2. **Everything is a File**: Unified interface makes operations simpler
3. **Dynamically Generated**: Every read returns the latest information
4. **Window to the Kernel**: Communicate with Kernel through file interface

### Practical Tips

```bash
# Quick system information
cat /proc/cpuinfo | grep "model name" | head -1
cat /proc/meminfo | grep MemTotal
cat /proc/version

# Monitor process
watch -n 1 'cat /proc/[PID]/status | grep VmRSS'

# Control hardware (requires root)
echo 1 > /sys/class/leds/*/brightness

# Generate test data
dd if=/dev/zero of=test.img bs=1M count=100
```

## Further Reading

- [The /proc Filesystem](https://www.kernel.org/doc/html/latest/filesystems/proc.html) - Official Kernel documentation
- [sysfs - The filesystem for exporting kernel objects](https://www.kernel.org/doc/html/latest/filesystems/sysfs.html)
- [Linux Device Drivers, 3rd Edition](https://lwn.net/Kernel/LDD3/) - Chapter 3: Character Drivers
- [Filesystem Hierarchy Standard](https://refspecs.linuxfoundation.org/FHS_3.0/fhs-3.0.html)

## Back to Main Chapter

← [Back to Chapter 2: Filesystem and Directory Structure](README.md)
