# 實驗：探索 Linux 檔案系統

本實驗將透過實際操作來驗證本章學到的概念。

## 實驗 1：深入探索虛擬檔案系統

### 目標
全面探索 `/proc`, `/sys`, `/dev` 三個虛擬檔案系統。

### 編譯與執行

**C 版本：**
```bash
cd codes/
gcc -o explore_virtual_fs explore_virtual_fs.c
./explore_virtual_fs
```

**Rust 版本：**
```bash
cd codes/
rustc explore_virtual_fs.rs
./explore_virtual_fs
```

### 程式功能詳解

這個進階程式展示了以下內容：

#### Part 1: 探索 /proc - 當前程序資訊
- 讀取程序 PID 和 PPID
- 讀取執行檔路徑 (`/proc/[pid]/exe`)
- 讀取命令列參數 (`/proc/[pid]/cmdline`)
- 讀取程序狀態 (`/proc/[pid]/status`)
- 列出開啟的檔案描述符 (`/proc/[pid]/fd/`)

#### Part 2: 探索 /proc - 系統資訊
- 讀取 CPU 資訊和核心數 (`/proc/cpuinfo`)
- 讀取記憶體資訊 (`/proc/meminfo`)
- 計算系統運行時間 (`/proc/uptime`)

#### Part 3: 探索 /sys - 硬體資訊
- 列出所有網路介面 (`/sys/class/net/`)
- 讀取每個介面的 MAC 地址、狀態、MTU
- 讀取 CPU 頻率 (`/sys/devices/system/cpu/`)

#### Part 4: 探索 /dev - 裝置檔案
- 檢查特殊裝置 (null, zero, random, urandom, tty)
- 測試從 `/dev/urandom` 讀取亂數
- 列出區塊裝置 (硬碟) 及其主要/次要編號

### 實作細節對比

**C 語言實作：**
- 使用 `fopen()`, `fgets()`, `readlink()` 讀取檔案
- 使用 `opendir()`, `readdir()` 列出目錄
- 使用 `stat()` 取得檔案類型和裝置編號
- 手動處理字串和緩衝區

**Rust 語言實作：**
- 使用 `fs::read_to_string()` 讀取檔案
- 使用 `fs::read_dir()` 列出目錄
- 使用 `fs::metadata()` 取得檔案資訊
- 使用 `FileTypeExt` trait 檢查裝置類型
- 自動記憶體管理和錯誤處理

### 觀察重點

1. **虛擬檔案系統的統一介面**
   - 三個不同的虛擬檔案系統
   - 都使用相同的檔案 I/O 介面
   - 不需要特殊的系統呼叫

2. **即時資訊生成**
   - 每次執行程式，數值都不同
   - 證明這些資料是動態生成的

3. **硬體控制介面**
   - `/sys` 的檔案通常是可寫的
   - 可以透過寫入檔案來控制硬體

4. **程式語言無關**
   - C 和 Rust 都能用標準檔案 I/O
   - 任何程式語言都可以存取這些資訊

## 實驗 2：探索 /dev 裝置檔案

### 目標
理解 Linux 把硬體裝置抽象成檔案。

### 指令

```bash
# 列出裝置
ls -la /dev | head -20

# 查看硬碟裝置
ls -la /dev/sd*

# 查看特殊裝置
ls -la /dev/null /dev/zero /dev/random
```

### 特殊裝置實驗

```bash
# /dev/null - 黑洞，丟掉一切
echo "Hello" > /dev/null    # 不會有任何輸出
cat /dev/null               # 空的

# /dev/zero - 無限的 0
dd if=/dev/zero bs=1 count=10 | xxd
# 輸出：10 個 0x00

# /dev/random - 亂數
dd if=/dev/random bs=16 count=1 | xxd
# 輸出：16 個隨機 bytes
```

### 裝置檔案類型

```bash
# 查看裝置類型
file /dev/sda     # block special (區塊裝置)
file /dev/tty     # character special (字元裝置)
file /dev/null    # character special
```

區塊裝置 vs 字元裝置：
- **區塊裝置 (b)**：可以隨機存取，有緩衝。例如：硬碟
- **字元裝置 (c)**：依序存取，無緩衝。例如：鍵盤、終端機

## 實驗 3：符號連結練習

### 目標
理解符號連結是「捷徑」，不是複製檔案。

### 建立符號連結

```bash
# 建立測試目錄
mkdir -p ~/symlink_test
cd ~/symlink_test

# 建立一個原始檔案
echo "Original content" > original.txt

# 建立符號連結
ln -s original.txt link_to_original.txt

# 查看連結
ls -la
# 輸出類似：
# -rw-rw-r-- 1 user user   17 ... original.txt
# lrwxrwxrwx 1 user user   12 ... link_to_original.txt -> original.txt
```

### 驗證連結行為

```bash
# 透過連結讀取內容（與原檔相同）
cat link_to_original.txt

# 修改原檔
echo "Modified content" > original.txt

# 連結也會反映變更
cat link_to_original.txt

# 刪除原檔
rm original.txt

# 連結變成「斷掉的連結」
cat link_to_original.txt  # 錯誤：No such file or directory
ls -la link_to_original.txt  # 顯示紅色（斷掉的連結）
```

### 系統中的符號連結範例

```bash
# /bin 通常是連結到 /usr/bin
ls -la /bin

# Python 版本管理常用符號連結
ls -la /usr/bin/python*
```

## 實驗 4：掛載點探索

### 目標
理解掛載是如何運作的。

### 查看掛載資訊

```bash
# 查看所有掛載點
mount

# 只看根目錄
mount | grep "on / "

# 更友善的格式
df -h

# 樹狀結構
findmnt
```

### 觀察虛擬檔案系統的掛載

```bash
# 查看 /proc 的掛載
mount | grep proc
# 輸出：proc on /proc type proc

# 查看 /sys 的掛載
mount | grep sysfs
# 輸出：sysfs on /sys type sysfs
```

這說明 /proc 和 /sys 是特殊的檔案系統類型，不是存在硬碟上的。

## 實驗 5：探索 /proc/[PID]

### 目標
為第三章「Process」做準備，先了解每個程序的 /proc 目錄。

### 查看 PID 1 (systemd)

```bash
# 列出 systemd 的 proc 目錄
ls /proc/1/

# 查看執行檔位置
ls -la /proc/1/exe

# 查看命令列參數
cat /proc/1/cmdline

# 查看環境變數（可能需要 sudo）
sudo cat /proc/1/environ | tr '\0' '\n' | head -10
```

### 查看自己的 shell

```bash
# 找出目前 shell 的 PID
echo $$

# 探索這個 PID 的資訊
ls /proc/$$/

# 查看狀態
cat /proc/$$/status | head -20

# 查看記憶體映射
cat /proc/$$/maps | head -10
```

### 觀察程序的檔案描述符

```bash
# 每個程序打開的檔案都會出現在這裡
ls -la /proc/$$/fd

# 0, 1, 2 分別是 stdin, stdout, stderr
```

## 實驗總結

| 實驗 | 學到的概念 | 程式語言 |
|------|-----------|---------|
| 深入虛擬檔案系統 | /proc, /sys, /dev 的全面應用 | C & Rust |
| 探索 /dev | Linux 把硬體抽象成檔案 | Bash |
| 符號連結 | 連結是「指標」不是複製 | Bash |
| 掛載點 | 檔案系統如何連接在一起 | Bash |
| /proc/[PID] | 程序資訊的存取方式（第三章預習）| Bash |

### 程式設計重點

**C 語言：**
- ✅ 底層控制、效能優異
- ✅ 直接使用 POSIX API
- ⚠️ 需要手動管理記憶體
- ⚠️ 需要仔細處理錯誤

**Rust 語言：**
- ✅ 記憶體安全、無資料競爭
- ✅ 現代化的錯誤處理
- ✅ 高效能（接近 C）
- ⚠️ 學習曲線較陡

### 通用概念

無論使用哪種程式語言：
1. 虛擬檔案系統使用標準的檔案 I/O
2. `open()` → `read()` → `close()` 的模式
3. 不需要特殊的系統呼叫或權限（大部分情況）
4. 體現 Unix "一切皆檔案" 哲學

## 清理

```bash
# 刪除測試目錄
rm -rf ~/symlink_test

# 刪除編譯的程式（可選）
cd codes/
rm -f explore_virtual_fs
```

## 下一步

這些概念將在第三章「Process」中繼續深入：
- 如何透過 /proc 監控程序
- 程序的記憶體映射 (/proc/[pid]/maps)
- 程序間的父子關係 (/proc/[pid]/status 的 PPid)
