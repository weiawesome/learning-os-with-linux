# Experiment: Exploring the Linux Filesystem

This experiment will verify the concepts learned in this chapter through hands-on practice.

## Experiment 1: Deep Dive into Virtual Filesystems

### Goal
Comprehensively explore all three virtual filesystems: `/proc`, `/sys`, and `/dev`.

### Compile and Run

**C Version:**
```bash
cd codes/
gcc -o explore_virtual_fs explore_virtual_fs.c
./explore_virtual_fs
```

**Rust Version:**
```bash
cd codes/
rustc explore_virtual_fs.rs
./explore_virtual_fs
```

### Program Features Explained

This advanced program demonstrates:

#### Part 1: Explore /proc - Current Process Information
- Read process PID and PPID
- Read executable path (`/proc/[pid]/exe`)
- Read command line arguments (`/proc/[pid]/cmdline`)
- Read process status (`/proc/[pid]/status`)
- List open file descriptors (`/proc/[pid]/fd/`)

#### Part 2: Explore /proc - System Information
- Read CPU info and core count (`/proc/cpuinfo`)
- Read memory information (`/proc/meminfo`)
- Calculate system uptime (`/proc/uptime`)

#### Part 3: Explore /sys - Hardware Information
- List all network interfaces (`/sys/class/net/`)
- Read MAC address, state, and MTU for each interface
- Read CPU frequency (`/sys/devices/system/cpu/`)

#### Part 4: Explore /dev - Device Files
- Check special devices (null, zero, random, urandom, tty)
- Test reading random data from `/dev/urandom`
- List block devices (disks) with major/minor numbers

### Implementation Comparison

**C Implementation:**
- Uses `fopen()`, `fgets()`, `readlink()` to read files
- Uses `opendir()`, `readdir()` to list directories
- Uses `stat()` to get file type and device numbers
- Manual string and buffer handling

**Rust Implementation:**
- Uses `fs::read_to_string()` to read files
- Uses `fs::read_dir()` to list directories
- Uses `fs::metadata()` to get file information
- Uses `FileTypeExt` trait to check device types
- Automatic memory management and error handling

### Key Observations

1. **Unified Interface Across Virtual Filesystems**
   - Three different virtual filesystems
   - All use the same file I/O interface
   - No special system calls needed

2. **Real-time Information Generation**
   - Values differ each time you run the program
   - Proves data is dynamically generated

3. **Hardware Control Interface**
   - Files in `/sys` are often writable
   - Can control hardware by writing to files

4. **Language Agnostic**
   - Both C and Rust use standard file I/O
   - Any programming language can access this information

## Experiment 2: Exploring /dev Device Files

### Goal
Understand how Linux abstracts hardware devices as files.

### Commands

```bash
# List devices
ls -la /dev | head -20

# View disk devices
ls -la /dev/sd*

# View special devices
ls -la /dev/null /dev/zero /dev/random
```

### Special Device Experiments

```bash
# /dev/null - Black hole, discards everything
echo "Hello" > /dev/null    # No output
cat /dev/null               # Empty

# /dev/zero - Infinite zeros
dd if=/dev/zero bs=1 count=10 | xxd
# Output: 10 x 0x00

# /dev/random - Random data
dd if=/dev/random bs=16 count=1 | xxd
# Output: 16 random bytes
```

### Device File Types

```bash
# Check device types
file /dev/sda     # block special
file /dev/tty     # character special
file /dev/null    # character special
```

Block device vs Character device:
- **Block device (b)**: Random access, buffered. Example: hard drives
- **Character device (c)**: Sequential access, unbuffered. Example: keyboard, terminal

## Experiment 3: Symbolic Link Practice

### Goal
Understand that symbolic links are "shortcuts", not file copies.

### Creating Symbolic Links

```bash
# Create test directory
mkdir -p ~/symlink_test
cd ~/symlink_test

# Create an original file
echo "Original content" > original.txt

# Create symbolic link
ln -s original.txt link_to_original.txt

# View the link
ls -la
# Output similar to:
# -rw-rw-r-- 1 user user   17 ... original.txt
# lrwxrwxrwx 1 user user   12 ... link_to_original.txt -> original.txt
```

### Verify Link Behavior

```bash
# Read content through link (same as original)
cat link_to_original.txt

# Modify original file
echo "Modified content" > original.txt

# Link reflects the change
cat link_to_original.txt

# Delete original file
rm original.txt

# Link becomes "broken"
cat link_to_original.txt  # Error: No such file or directory
ls -la link_to_original.txt  # Shows in red (broken link)
```

### Symbolic Links in the System

```bash
# /bin is usually a link to /usr/bin
ls -la /bin

# Python version management often uses symbolic links
ls -la /usr/bin/python*
```

## Experiment 4: Mount Point Exploration

### Goal
Understand how mounting works.

### View Mount Information

```bash
# View all mount points
mount

# View root directory only
mount | grep "on / "

# Friendlier format
df -h

# Tree structure
findmnt
```

### Observe Virtual Filesystem Mounts

```bash
# View /proc mount
mount | grep proc
# Output: proc on /proc type proc

# View /sys mount
mount | grep sysfs
# Output: sysfs on /sys type sysfs
```

This shows that /proc and /sys are special filesystem types, not stored on disk.

## Experiment 5: Exploring /proc/[PID]

### Goal
Prepare for Chapter 3 "Process" by understanding each process's /proc directory.

### View PID 1 (systemd)

```bash
# List systemd's proc directory
ls /proc/1/

# View executable location
ls -la /proc/1/exe

# View command line arguments
cat /proc/1/cmdline

# View environment variables (may need sudo)
sudo cat /proc/1/environ | tr '\0' '\n' | head -10
```

### View Your Own Shell

```bash
# Find current shell's PID
echo $$

# Explore this PID's information
ls /proc/$$/

# View status
cat /proc/$$/status | head -20

# View memory mappings
cat /proc/$$/maps | head -10
```

### Observe Process File Descriptors

```bash
# Every file opened by a process appears here
ls -la /proc/$$/fd

# 0, 1, 2 are stdin, stdout, stderr respectively
```

## Experiment Summary

| Experiment | Concept Learned | Language |
|------------|-----------------|----------|
| Deep dive virtual fs | Comprehensive /proc, /sys, /dev usage | C & Rust |
| Exploring /dev | Linux abstracts hardware as files | Bash |
| Symbolic links | Links are "pointers" not copies | Bash |
| Mount points | How filesystems connect together | Bash |
| /proc/[PID] | How to access process information (Ch.3 preview) | Bash |

### Programming Highlights

**C Language:**
- ✅ Low-level control, excellent performance
- ✅ Direct POSIX API usage
- ⚠️ Manual memory management required
- ⚠️ Careful error handling needed

**Rust Language:**
- ✅ Memory safety, no data races
- ✅ Modern error handling
- ✅ High performance (near C)
- ⚠️ Steeper learning curve

### Universal Concepts

Regardless of programming language:
1. Virtual filesystems use standard file I/O
2. `open()` → `read()` → `close()` pattern
3. No special system calls or permissions needed (usually)
4. Embodies Unix "everything is a file" philosophy

## Cleanup

```bash
# Delete test directory
rm -rf ~/symlink_test

# Delete compiled programs (optional)
cd codes/
rm -f explore_virtual_fs
```

## Next Steps

These concepts will be explored further in Chapter 3 "Process":
- How to monitor processes through /proc
- Process memory mappings (/proc/[pid]/maps)
- Parent-child relationships between processes (/proc/[pid]/status PPid)
