# 虛擬檔案系統深入解析

<div align="center">

[📖 中文](virtual-filesystem.zh.md) | [📖 English](virtual-filesystem.md)

</div>

## 什麼是虛擬檔案系統？

虛擬檔案系統（Virtual Filesystem）是一種特殊的檔案系統，它的內容**不存在於硬碟上**，而是由 Kernel 在記憶體中**即時生成**。

### 為什麼需要虛擬檔案系統？

Linux 的哲學是「一切皆檔案」（Everything is a file），包括：
- 硬體裝置
- 程序資訊
- Kernel 狀態
- 系統設定

使用檔案介面的好處：
- 統一的操作方式（read、write、open、close）
- 可以用標準工具（cat、echo、grep）
- 程式不需要特殊的 API

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
│        ├──→ /proc  → procfs → 即時程序資訊            │
│        ├──→ /sys   → sysfs  → 即時系統資訊            │
│        └──→ /dev   → devfs  → 對應硬體裝置            │
│                                                     │
│   註：VFS 也處理真實檔案系統（如：ext4, xfs）            │
│                                                     │
└─────────────────────────────────────────────────────┘
```

## /proc - 程序與 Kernel 資訊

`/proc` 是 **proc**ess **f**ile**s**ystem 的縮寫，提供：
- 所有執行中程序的資訊
- Kernel 運行狀態
- 系統硬體資訊

### /proc 的檔案類型

```bash
ls -la /proc/
```

你會看到：
- **數字目錄** (`1`, `1234`, `5678`...): 每個數字代表一個程序的 PID
- **一般檔案** (`cpuinfo`, `meminfo`, `version`...): 系統資訊

### 系統層級資訊

```bash
# === CPU 資訊 ===
cat /proc/cpuinfo
# 顯示：CPU 型號、核心數、頻率、cache 大小

# === 記憶體資訊 ===
cat /proc/meminfo
# 顯示：總記憶體、可用記憶體、swap 使用量

# === Kernel 版本 ===
cat /proc/version
# 顯示：Linux 版本、GCC 版本、編譯時間

# === 系統運行時間 ===
cat /proc/uptime
# 格式：[總運行秒數] [閒置時間秒數]

# === 載入的模組 ===
cat /proc/modules
# 列出所有已載入的 kernel 模組

# === 檔案系統類型 ===
cat /proc/filesystems
# 列出 kernel 支援的檔案系統

# === 分區資訊 ===
cat /proc/partitions
# 列出所有硬碟分區

# === 網路統計 ===
cat /proc/net/dev
# 顯示網路介面的傳輸統計
```

### 程序層級資訊：/proc/[PID]

每個執行中的程序都有一個 `/proc/[PID]` 目錄：

```bash
# 列出 PID 1 的目錄內容
ls -la /proc/1/

# 你會看到：
# cmdline  - 啟動命令
# environ  - 環境變數
# exe      - 執行檔的符號連結
# fd/      - 開啟的檔案描述符
# maps     - 記憶體映射
# status   - 程序狀態摘要
# stat     - 詳細統計資訊
# cwd      - 當前工作目錄的符號連結
```

#### 實用範例

```bash
# === 查看程序的執行檔位置 ===
ls -la /proc/1/exe
# 例如：/proc/1/exe -> /usr/lib/systemd/systemd

# === 查看程序的啟動命令 ===
cat /proc/1/cmdline | tr '\0' ' '
# 注意：cmdline 用 null byte 分隔參數，用 tr 轉成空格

# === 查看程序的環境變數 ===
cat /proc/1/environ | tr '\0' '\n'

# === 查看程序開啟的檔案 ===
ls -la /proc/1/fd/
# 0 -> stdin
# 1 -> stdout
# 2 -> stderr
# 3, 4, 5... -> 其他開啟的檔案

# === 查看程序的記憶體映射 ===
cat /proc/1/maps
# 顯示程式碼段、資料段、共享函式庫的記憶體位置

# === 查看程序狀態 ===
cat /proc/1/status
# 顯示：Name, State, PID, PPID, Threads, 記憶體使用...
```

### /proc/self - 當前程序

`/proc/self` 是一個特殊的符號連結，永遠指向**當前程序**：

```bash
# 查看 /proc/self 指向哪裡
ls -la /proc/self
# 例如：/proc/self -> 12345

# 每次執行時 PID 都不同
echo $$           # 顯示當前 shell 的 PID
ls -la /proc/self # 會指向同一個 PID
```

### 透過 /proc 修改 Kernel 參數（/proc/sys）

`/proc/sys` 目錄允許你**讀取和修改** Kernel 參數：

```bash
# === 網路參數 ===
# 查看 IP 轉發是否啟用
cat /proc/sys/net/ipv4/ip_forward

# 啟用 IP 轉發（需要 root）
echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward

# === 核心參數 ===
# 查看主機名稱
cat /proc/sys/kernel/hostname

# 查看系統允許的最大 PID
cat /proc/sys/kernel/pid_max

# === 虛擬記憶體參數 ===
# 查看 swap 傾向性 (0-100)
cat /proc/sys/vm/swappiness
```

注意：透過 `/proc/sys` 的修改在**重開機後會消失**。永久修改需要編輯 `/etc/sysctl.conf`。

### 實戰：分析正在執行的程序

讓我們分析一個實際的程序（例如 terminal）：

```bash
# 1. 找到你的 terminal 的 PID
echo $$  # 或用 ps aux | grep bash

# 2. 假設 PID 是 12345，查看詳細資訊
PID=12345

# 執行檔
ls -la /proc/$PID/exe

# 工作目錄
ls -la /proc/$PID/cwd

# 開啟的檔案
ls -la /proc/$PID/fd/

# 記憶體使用
cat /proc/$PID/status | grep -E "VmSize|VmRSS"
```

## /sys - 系統與硬體資訊

`/sys` 是 **sys**tem **f**ile**s**ystem 的縮寫，提供：
- 硬體裝置資訊
- Kernel 物件模型
- 裝置驅動程式介面
- 硬體控制與設定

### /sys 的結構

```
/sys/
├── block/        → 區塊裝置（硬碟、SSD）
├── bus/          → 匯流排類型（USB, PCI, I2C...）
├── class/        → 裝置類別（network, input, leds...）
├── devices/      → 所有裝置的樹狀結構
├── firmware/     → 韌體介面
├── fs/           → 檔案系統資訊
├── kernel/       → Kernel 內部資訊
├── module/       → 已載入的模組
└── power/        → 電源管理
```

### 常用範例

```bash
# === 網路裝置 ===
# 列出所有網路介面
ls /sys/class/net/
# 例如：eth0, wlan0, lo

# 查看網路介面的 MAC 位址
cat /sys/class/net/eth0/address

# 查看網路介面狀態（up/down）
cat /sys/class/net/eth0/operstate

# 查看網路速度
cat /sys/class/net/eth0/speed

# === CPU 資訊 ===
# 列出所有 CPU
ls /sys/devices/system/cpu/
# 例如：cpu0, cpu1, cpu2...

# 查看 CPU 頻率
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq

# 查看 CPU 支援的頻率範圍
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies

# === 區塊裝置（硬碟）===
# 列出所有區塊裝置
ls /sys/block/
# 例如：sda, sdb, nvme0n1

# 查看硬碟大小（單位：512 byte 扇區）
cat /sys/block/sda/size

# 查看硬碟型號
cat /sys/block/sda/device/model

# === 背光控制 ===
# 查看螢幕亮度（筆電）
cat /sys/class/backlight/*/brightness
cat /sys/class/backlight/*/max_brightness

# 調整亮度（需要 root）
echo 500 | sudo tee /sys/class/backlight/*/brightness

# === LED 控制 ===
# 列出系統 LED
ls /sys/class/leds/

# 控制 LED（需要 root）
echo 1 | sudo tee /sys/class/leds/input3::capslock/brightness
echo 0 | sudo tee /sys/class/leds/input3::capslock/brightness
```

### /sys vs /proc 的差異

| 特性 | /proc | /sys |
|------|-------|------|
| 主要用途 | 程序資訊、Kernel 狀態 | 硬體裝置、驅動程式 |
| 內容格式 | 較自由，有時是多行 | 嚴格，通常一個檔案一個值 |
| 可寫性 | 部分可寫（/proc/sys） | 許多檔案可寫（控制硬體）|
| 引入時間 | Linux 1.0 (1994) | Linux 2.6 (2003) |
| 設計理念 | 提供程序資訊 | 統一裝置模型 |

## /dev - 裝置檔案

`/dev` 是 **dev**ice 的縮寫，包含所有硬體裝置的特殊檔案。

### 裝置檔案的本質

在 Linux 中，硬體裝置被抽象成檔案，使用標準的檔案操作：

```c
// 開啟裝置
int fd = open("/dev/sda", O_RDONLY);

// 讀取硬碟
read(fd, buffer, size);

// 關閉裝置
close(fd);
```

### 裝置類型

```bash
ls -la /dev/ | head -20
```

觀察第一個字元：
- `b` = **區塊裝置**（Block Device）
- `c` = **字元裝置**（Character Device）

#### 區塊裝置 (Block Device)

**特性**：以固定大小的區塊讀寫（通常 512 bytes 或 4KB）

```bash
# 硬碟與分區
ls -la /dev/sd*
# sda      - 第一顆 SATA/SCSI 硬碟
# sda1     - 第一個分區
# sda2     - 第二個分區
# sdb      - 第二顆硬碟

# NVMe SSD
ls -la /dev/nvme*
# nvme0n1     - 第一顆 NVMe SSD
# nvme0n1p1   - 第一個分區

# 迴圈裝置（loop device）
ls -la /dev/loop*
# 用於掛載 ISO 映像檔
```

#### 字元裝置 (Character Device)

**特性**：以字元（byte）串流方式讀寫

```bash
# === 特殊裝置 ===
ls -la /dev/null     # 黑洞：丟掉所有輸入
ls -la /dev/zero     # 無限產生 0
ls -la /dev/random   # 亂數產生器（會阻塞）
ls -la /dev/urandom  # 亂數產生器（不阻塞）
ls -la /dev/full     # 永遠回報「磁碟滿」

# === 終端機裝置 ===
ls -la /dev/tty*     # 終端機
ls -la /dev/pts/*    # 偽終端機（SSH、GUI terminal）

# === 輸入裝置 ===
ls -la /dev/input/*  # 鍵盤、滑鼠、觸控板
```

### 常見裝置用途

#### /dev/null - 資料黑洞

```bash
# 丟棄標準錯誤輸出
command 2>/dev/null

# 丟棄所有輸出
command &>/dev/null

# 清空檔案內容
cat /dev/null > file.txt

# 測試寫入效能（不實際寫入硬碟）
dd if=/dev/zero of=/dev/null bs=1M count=1000
```

#### /dev/zero - 產生零

```bash
# 建立 1GB 的空檔案
dd if=/dev/zero of=1GB.img bs=1M count=1024

# 覆寫硬碟（銷毀資料）
sudo dd if=/dev/zero of=/dev/sdb bs=1M
```

#### /dev/random 與 /dev/urandom - 亂數

```bash
# 產生 32 bytes 的亂數
head -c 32 /dev/urandom | base64

# 產生隨機密碼
tr -dc A-Za-z0-9 </dev/urandom | head -c 16

# 產生隨機檔案
dd if=/dev/urandom of=random.bin bs=1M count=10
```

#### /dev/tty - 終端機

```bash
# 查看當前終端機
tty
# 例如：/dev/pts/0

# 直接寫入到你的終端機
echo "Hello!" > /dev/tty

# 從另一個終端機傳訊息（需要權限）
echo "Hi there!" > /dev/pts/1
```

### 裝置的主要與次要編號

```bash
ls -la /dev/sda*
```

輸出類似：
```
brw-rw---- 1 root disk 8, 0 Jan 24 10:00 /dev/sda
brw-rw---- 1 root disk 8, 1 Jan 24 10:00 /dev/sda1
brw-rw---- 1 root disk 8, 2 Jan 24 10:00 /dev/sda2
                       ↑  ↑
                    主要 次要
```

- **主要編號**（Major）：識別裝置驅動程式（例如 8 = SCSI 磁碟）
- **次要編號**（Minor）：識別具體裝置（例如 0 = sda, 1 = sda1）

### udev - 動態裝置管理

`/dev` 目錄由 `udev` 服務管理：

```bash
# 查看 udev 規則
ls /etc/udev/rules.d/
ls /lib/udev/rules.d/

# 觸發 udev 事件
sudo udevadm trigger

# 監控 udev 事件（插入 USB 時會顯示）
sudo udevadm monitor
```

當你插入 USB 隨身碟：
1. Kernel 偵測到新裝置
2. 發送事件給 udev
3. udev 建立 `/dev/sdb`（或其他名稱）
4. udev 可能自動掛載到 `/media/`

## 實戰範例

### 範例 1：監控 CPU 溫度

```bash
# 查看溫度感測器
cat /sys/class/thermal/thermal_zone*/temp

# 轉換為攝氏度（許多系統以 millidegree 為單位）
awk '{print $1/1000 "°C"}' /sys/class/thermal/thermal_zone0/temp

# 持續監控
watch -n 1 'cat /sys/class/thermal/thermal_zone0/temp | awk "{print \$1/1000 \"°C\"}"'
```

### 範例 2：分析記憶體使用

```bash
# 查看總覽
free -h

# 詳細資訊
cat /proc/meminfo

# 找出最耗記憶體的程序
ps aux --sort=-%mem | head -10

# 查看某程序的詳細記憶體
cat /proc/[PID]/status | grep -E "Vm|Rss"
```

### 範例 3：網路介面資訊

```bash
# 列出所有網路介面
ip link show
# 或
ls /sys/class/net/

# 查看介面統計
cat /proc/net/dev

# 查看某介面的詳細資訊
IFACE=eth0
echo "MAC: $(cat /sys/class/net/$IFACE/address)"
echo "MTU: $(cat /sys/class/net/$IFACE/mtu)"
echo "State: $(cat /sys/class/net/$IFACE/operstate)"
```

### 範例 4：探索 USB 裝置

```bash
# 列出 USB 裝置
lsusb

# 詳細資訊
lsusb -v

# 透過 /sys 查看
ls /sys/bus/usb/devices/

# 查看某個 USB 裝置的資訊
cat /sys/bus/usb/devices/1-1/manufacturer
cat /sys/bus/usb/devices/1-1/product
```

## 程式範例

想要實際編寫程式來探索虛擬檔案系統嗎？

請參閱 [實驗文件](./experiment.zh.md) 中的「實驗 1：深入探索虛擬檔案系統」，其中包含：
- 完整的 C 語言實作
- 完整的 Rust 語言實作
- 詳細的程式碼說明
- 編譯和執行指引

## 總結

### 三個虛擬檔案系統的對比

| 特性 | /proc | /sys | /dev |
|------|-------|------|------|
| **主要用途** | 程序與 Kernel 資訊 | 硬體與驅動程式 | 裝置存取介面 |
| **內容來源** | Kernel 即時生成 | Kernel 裝置模型 | udev 動態建立 |
| **是否可寫** | 部分可寫 (/proc/sys) | 許多可寫 | 可讀寫（實際裝置）|
| **資料格式** | 人類可讀，多行 | 一檔一值，嚴格格式 | 二進位或字元流 |
| **典型操作** | 讀取資訊 | 讀取與控制硬體 | I/O 操作 |

### 關鍵概念

1. **虛擬 ≠ 不真實**：這些資訊都是真實的，只是不儲存在硬碟上
2. **一切皆檔案**：統一介面讓操作更簡單
3. **動態生成**：每次讀取都是最新資訊
4. **Kernel 的窗口**：透過檔案介面與 Kernel 溝通

### 實用技巧

```bash
# 快速查看系統資訊
cat /proc/cpuinfo | grep "model name" | head -1
cat /proc/meminfo | grep MemTotal
cat /proc/version

# 監控程序
watch -n 1 'cat /proc/[PID]/status | grep VmRSS'

# 控制硬體（需要 root）
echo 1 > /sys/class/leds/*/brightness

# 產生測試資料
dd if=/dev/zero of=test.img bs=1M count=100
```

## 返回主章節

← [返回第二章：檔案系統與目錄結構](README.zh.md)
