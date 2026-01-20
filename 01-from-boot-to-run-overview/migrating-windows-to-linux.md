# Migrating Windows to Linux

<div align="center">

[📖 中文](migrating-windows-to-linux.zh.md) | [📖 English](migrating-windows-to-linux.md)

</div>

## Before You Begin – Steps to Install Linux (Pop!_OS) Over Windows

1. Prepare the OS image

Download the [Pop!_OS](https://system76.com/pop/download/) ISO file.
Choose the correct ISO for your system, e.g.: Pop!_OS 26.04 LTS amd64 ISO.

2. Create a Bootable USB Drive

Use [Rufus](https://rufus.ie/) to make your bootable USB drive.
![](https://rufus.ie/pics/screenshot1_en.png)

Honestly, it's quite straightforward: just select your USB drive, pick the ISO file, press "Start," and you're done.

3. Enter UEFI to Change Boot Order

Restart your computer, and quickly press F12 to enter the UEFI setup (the exact key varies by brand).
![](https://km-ap.asus.com/uploads/PhotoLibrarys/8a53fc79-6a00-4a96-8408-a7e886967aef/20240530113830775_EN_1.png)

Select "Boot Priority," drag the USB drive to the top, then press F10 to save and exit.

```
Tip:

    BIOS vs UEFI
    These are two different firmware standards:
    Older PCs (Before 2010)        Modern PCs (After 2010)
    ┌──────────────┐            ┌──────────────┐
    │     BIOS     │            │     UEFI     │
    │ (Old Standard)│           │ (New Standard)│
    └──────────────┘            └──────────────┘

1. BIOS (Basic Input/Output System)

- Technology from the 1970s
- 16-bit mode
- Can only boot from MBR partition tables
- Typically uses a blue-background/white-text UI
- Fewer features

2. UEFI (Unified Extensible Firmware Interface)

- Developed in the 2000s to replace BIOS
- 32/64-bit mode
- Supports GPT partition tables
- Can have graphical interface and mouse support
- More powerful features (Secure Boot, network boot, etc.)

When you enter the setup screen
When you boot up and press F2, Del, F12, etc. to enter the firmware settings:
If your computer was bought after 2012, it's 99% likely to be a UEFI setup screen.
However! Many manufacturers still call it "BIOS Setup" so users are familiar with the terminology.
```

4. Boot, Install, Done – That's it!

![](https://cdn11.bigcommerce.com/s-pywjnxrcr2/images/stencil/original/image-manager/pop-customization-examplesr2.png)