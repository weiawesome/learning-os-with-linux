# Migrating Windows to Linux

<div align="center">

[📖 中文](migrating-windows-to-linux.zh.md) | [📖 English](migrating-windows-to-linux.md)

</div>

## 開始之前 - Windows 改灌到 Linux(pop_os) 的流程
1. 準備 OS 檔案

下載 [Pop!_OS](https://system76.com/pop/download/) 的 ISO 檔案
選擇對應的 ISO 檔案下載 例如：Pop!_OS 26.04 LTS amd64 的 ISO 檔案

2. 製作 開機隨身碟

使用 [Rufus](https://rufus.ie/) 製作 開機隨身碟
![](https://rufus.ie/pics/screenshot1_en.png)

說實在話 真的蠻無腦的 就選擇隨身碟 選擇 ISO 檔案 按下開始 就完成了

3. 進入 UEFI 設定開機順序

重開機後 快速按 F12 進入 UEFI 設定 (不同品牌電腦按鍵不同)
![](https://km-ap.asus.com/uploads/PhotoLibrarys/8a53fc79-6a00-4a96-8408-a7e886967aef/20240530113830775_EN_1.png)

選擇 Boot Priority 透過拖曳 USB 隨身碟到最上方 按下 F10 保存並退出

```
小知識: 

    BIOS vs UEFI
    這是兩種不同的韌體標準：
    傳統電腦 (2010年前)          現代電腦 (2010年後)
    ┌──────────────┐            ┌──────────────┐
    │     BIOS     │            │     UEFI     │
    │   (舊標準)    │            │   (新標準)    │
    └──────────────┘            └──────────────┘

1. BIOS (Basic Input/Output System)

- 1970年代的技術
- 16位元模式
- 只能從 MBR 分割表開機
- 介面通常是藍底白字的文字畫面
- 功能較少

2. UEFI (Unified Extensible Firmware Interface)

- 2000年代開發，取代 BIOS
- 32/64位元模式
- 支援 GPT 分割表
- 可以有圖形介面、滑鼠操作
- 功能更強大（安全開機、網路開機等）

你進入的設定畫面
當你開機按 F2、Del、F12 等進入設定畫面：
如果你的電腦是 2012 年後買的，99% 是進入 UEFI 設定畫面
但是！ 很多廠商為了讓使用者習慣，還是把它叫做 "BIOS Setup" 或 "BIOS 設定"
```


4. 開機 安裝 完成 ～ 就這樣

![](https://cdn11.bigcommerce.com/s-pywjnxrcr2/images/stencil/original/image-manager/pop-customization-examplesr2.png)