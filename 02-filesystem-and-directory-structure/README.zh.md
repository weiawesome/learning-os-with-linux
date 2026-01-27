# 檔案系統與目錄結構

<div align="center">

[📖 中文](README.zh.md) | [📖 English](README.md)

</div>

## 延續第一章：開機後的檔案系統掛載

在[第一章](../01-from-boot-to-run-overview/README.zh.md)中，我們提到 Kernel 啟動後會「掛載根檔案系統」。這一章，我們來深入了解這個過程以及 Linux 的目錄結構。

```
Kernel 開始執行 (在記憶體中)
  - 初始化硬體
  - 設定記憶體管理
  - 掛載根檔案系統      ← 本章重點
  ↓
系統就緒！
```

### 什麼是「掛載」？

掛載（mount）就像是把一個儲存裝置「接上」系統，讓你可以存取裡面的檔案。

生活比喻：
- 硬碟就像一個裝滿文件的保險箱
- 掛載就像把保險箱打開並放到你的桌上
- 掛載點就是你桌上放保險箱的位置

### 驗證當前掛載

```bash
# 查看根目錄掛載
mount | grep "on / "

# 查看所有掛載點
df -h
```

### initramfs 到真正根檔案系統的切換

開機時的掛載順序：

```
1. GRUB 載入 initramfs (臨時檔案系統)
   - 這是一個壓縮在記憶體中的迷你 Linux
   - 包含必要的驅動程式
   ↓
2. initramfs 載入硬碟驅動程式
   - 現在才能讀取真正的硬碟
   ↓
3. 切換到真正的根檔案系統
   - 從 initramfs 切換到硬碟上的 /
   - 這個過程叫做 "pivot_root" 或 "switch_root"
```

為什麼需要 initramfs？
- Kernel 本身不包含所有硬碟驅動程式
- initramfs 提供一個臨時環境來載入必要的驅動
- 載入驅動後，才能存取真正的根檔案系統

## Linux 目錄階層標準 (FHS)

FHS (Filesystem Hierarchy Standard) 定義了 Linux 目錄結構的標準。

```
/                           ← 根目錄，一切的起點
├── bin/      → 基本指令 (ls, cp, cat...)
├── boot/     → 開機檔案 (vmlinuz, initramfs)  ← 第一章提過
├── dev/      → 裝置檔案 (硬碟、終端機...)
├── etc/      → 系統設定檔 (設定檔集中地)
├── home/     → 使用者家目錄 (/home/你的名字)
├── lib/      → 系統函式庫 (共享程式碼)
├── media/    → 可移除媒體 (USB、光碟)
├── mnt/      → 臨時掛載點
├── opt/      → 第三方軟體
├── proc/     → 程序資訊 (虛擬檔案系統)      ← 重點！
├── root/     → root 使用者的家目錄
├── run/      → 執行時資訊
├── sbin/     → 系統管理指令 (需要 root)
├── sys/      → 系統資訊 (虛擬檔案系統)      ← 重點！
├── tmp/      → 暫存檔案 (重開機會清空)
├── usr/      → 使用者程式 (大部分軟體在這)
│   ├── bin/  → 使用者指令
│   ├── lib/  → 程式函式庫
│   └── share/→ 共享資料
└── var/      → 變動資料 (log、cache、mail)
    └── log/  → 系統日誌
```

### 常用目錄記憶口訣

| 目錄 | 記憶法 | 用途 |
|------|--------|------|
| `/bin` | **bin**ary | 基本執行檔 |
| `/etc` | **et c**etera (其他) | 設定檔 |
| `/home` | 家 | 使用者資料 |
| `/tmp` | **t**e**mp**orary | 暫存 |
| `/var` | **var**iable | 變動資料 |
| `/usr` | **U**nix **S**ystem **R**esources | 使用者程式 |

### 驗證目錄結構

```bash
# 列出根目錄
ls -la /

# 以樹狀結構顯示（只顯示第一層）
tree -L 1 /
```

## 虛擬檔案系統：/proc, /sys, /dev

這三個目錄很特別：它們不是真正存在於硬碟上的檔案，而是 Kernel 提供的「窗口」。

```
┌─────────────────────────────────────────────────────┐
│                     User Space                      │
│                                                     │
│   Want to know CPU info?                            │
│        │                                            │
│        ↓                                            │
│   cat /proc/cpuinfo                                 │
│        │                                            │
└────────┼────────────────────────────────────────────┘
         │
═════════╪══════════════════════════════════════════════
         │ (看起來像讀檔案，其實是詢問 Kernel)
         ↓
┌─────────────────────────────────────────────────────┐
│                   Kernel Space                      │
│                                                     │
│   Receives the request to read /proc/cpuinfo        │
│        │                                            │
│        ↓                                            │
│   Kernel generates CPU information on the fly       │
│   (This data doesn't physically exist on disk)      │
│                                                     │
└─────────────────────────────────────────────────────┘
```

### 快速概覽

| 目錄 | 用途 | 範例 |
|------|------|------|
| `/proc` | 程序與 Kernel 資訊 | `cat /proc/cpuinfo` |
| `/sys` | 硬體與驅動程式 | `ls /sys/class/net` |
| `/dev` | 裝置檔案 | `ls -la /dev/sda` |

### 生活比喻：汽車儀表板

想像你的電腦是一台汽車：
- **硬碟上的檔案** = 車裡放的東西（行李、文件）
- **/proc** = 儀表板顯示的即時資訊（時速、油量、引擎轉速）
- **/sys** = 控制面板（可以調整設定）
- **/dev** = 方向盤、油門、煞車（與硬體互動的介面）

### 基本驗證指令

```bash
# /proc - 查看系統資訊
cat /proc/version          # Kernel 版本
cat /proc/cpuinfo | head   # CPU 資訊
cat /proc/meminfo | head   # 記憶體資訊

# /proc/[PID] - 查看程序資訊
ls /proc/1/                # PID 1 的資訊
ls -la /proc/1/exe         # 執行檔位置

# /sys - 查看硬體資訊
ls /sys/class              # 硬體類別
ls /sys/class/net          # 網路介面

# /dev - 查看裝置
ls -la /dev/sda            # 第一顆硬碟
ls -la /dev/null           # 黑洞裝置
```

> 💡 **深入閱讀**：想了解更多關於虛擬檔案系統的詳細內容，包括如何透過它們控制硬體、監控系統、編寫程式存取等，請參閱：[虛擬檔案系統深入解析](./virtual-filesystem.zh.md)

## 檔案類型

在 Linux 中，「一切皆檔案」。用 `ls -l` 可以看到不同類型：

```bash
ls -la /
```

輸出的第一個字元代表檔案類型：

```
drwxr-xr-x   - bin/        d = 目錄 (directory)
-rw-r--r--   - some_file   - = 一般檔案 (regular file)
lrwxrwxrwx   - lib -> ...  l = 符號連結 (symbolic link)
brw-rw----   - /dev/sda    b = 區塊裝置 (block device)
crw-rw-rw-   - /dev/null   c = 字元裝置 (character device)
```

### 檔案類型說明

| 符號 | 類型 | 說明 | 範例 |
|------|------|------|------|
| `-` | 一般檔案 | 文字、程式、圖片等 | `/etc/passwd` |
| `d` | 目錄 | 資料夾 | `/home` |
| `l` | 符號連結 | 捷徑/軟連結 | `/lib` → `/usr/lib` |
| `b` | 區塊裝置 | 硬碟、USB | `/dev/sda` |
| `c` | 字元裝置 | 終端機、鍵盤 | `/dev/tty` |
| `p` | 具名管道 | 程序間通訊 | FIFO |
| `s` | Socket | 網路通訊 | `/run/docker.sock` |

### 符號連結範例

```bash
# 建立符號連結
ln -s /usr/bin/python3 ~/my_python

# 查看連結指向
ls -la ~/my_python

# 使用連結
~/my_python --version

# 刪除連結（不影響原檔案）
rm ~/my_python
```

## 驗證指令總整理

```bash
# === 查看掛載 ===
mount | head -10
df -h
findmnt /

# === 探索目錄 ===
ls -la /
tree -L 1 /

# === 虛擬檔案系統（基本）===
cat /proc/version
cat /proc/cpuinfo | head -10
cat /proc/meminfo | head -5
ls /sys/class
ls -la /dev | head -15

# 更多虛擬檔案系統命令請見：virtual-filesystem.zh.md

# === 檔案類型 ===
file /bin/ls           # 查看檔案類型
file /dev/null
file /proc/cpuinfo
```

## 與第一章的連結

| 第一章提到的 | 本章深入的 |
|-------------|-----------|
| Kernel 掛載根檔案系統 | 詳解掛載過程和 initramfs |
| /boot 目錄有 vmlinuz | FHS 完整目錄結構 |
| Kernel 在記憶體中運行 | /proc 即時顯示 Kernel 資訊 |

## 為第三章做準備

本章介紹的內容將在第三章「Process」中用到：

- `/proc/[pid]` - 每個程序的詳細資訊
- `/bin`, `/usr/bin` - 執行檔存放位置
- `/dev/tty` - 程序的終端機

下一章我們將深入了解「程序是什麼」以及如何管理它們！

## 實驗

詳見：[實驗](./experiment.zh.md)
