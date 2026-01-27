/**
 * explore_virtual_fs.rs - 深入探索 Linux 虛擬檔案系統
 * 
 * 本程式示範如何透過程式存取 /proc, /sys, /dev 這三個虛擬檔案系統
 * 
 * 編譯: rustc explore_virtual_fs.rs
 * 執行: ./explore_virtual_fs
 */

use std::fs;
use std::io::{self, Read};
use std::path::Path;

// ANSI 顏色碼
const COLOR_RESET: &str = "\x1b[0m";
const COLOR_BLUE: &str = "\x1b[34m";
const COLOR_GREEN: &str = "\x1b[32m";
const COLOR_YELLOW: &str = "\x1b[33m";
const COLOR_CYAN: &str = "\x1b[36m";

fn print_section(title: &str) {
    println!("\n{}╔═══════════════════════════════════════════════════════════╗{}", 
             COLOR_BLUE, COLOR_RESET);
    println!("{}║ {:<57} ║{}", COLOR_BLUE, title, COLOR_RESET);
    println!("{}╚═══════════════════════════════════════════════════════════╝{}\n", 
             COLOR_BLUE, COLOR_RESET);
}

fn print_item(label: &str, value: &str) {
    println!("{}{:<20}:{} {}", COLOR_CYAN, label, COLOR_RESET, value);
}

// ============================================================================
// Part 1: 探索 /proc - 程序與核心資訊
// ============================================================================

fn explore_proc_current_process() {
    print_section("Part 1: 探索 /proc - 當前程序資訊");
    
    let pid = std::process::id();
    
    println!("當前程序資訊：");
    println!("  PID:  {}\n", pid);
    
    // 讀取執行檔路徑
    let exe_path = format!("/proc/{}/exe", pid);
    if let Ok(target) = fs::read_link(&exe_path) {
        print_item("執行檔路徑", target.to_string_lossy().as_ref());
    }
    
    // 讀取命令列
    let cmdline_path = format!("/proc/{}/cmdline", pid);
    if let Ok(content) = fs::read_to_string(&cmdline_path) {
        let cmdline: String = content.chars()
            .map(|c| if c == '\0' { ' ' } else { c })
            .collect();
        print_item("命令列", cmdline.trim());
    }
    
    // 讀取狀態
    let status_path = format!("/proc/{}/status", pid);
    if let Ok(content) = fs::read_to_string(&status_path) {
        println!("\n{}重要狀態資訊:{}", COLOR_GREEN, COLOR_RESET);
        for line in content.lines() {
            if line.starts_with("State:") || 
               line.starts_with("Threads:") ||
               line.starts_with("VmSize:") ||
               line.starts_with("VmRSS:") ||
               line.starts_with("VmData:") {
                println!("  {}", line);
            }
        }
    }
    
    // 列出開啟的檔案描述符
    println!("\n{}開啟的檔案描述符 (前 5 個):{}", COLOR_GREEN, COLOR_RESET);
    let fd_dir = format!("/proc/{}/fd", pid);
    if let Ok(entries) = fs::read_dir(&fd_dir) {
        let mut count = 0;
        for entry in entries.flatten() {
            if count >= 5 { break; }
            if let Ok(target) = fs::read_link(entry.path()) {
                println!("  fd {} -> {}", 
                         entry.file_name().to_string_lossy(),
                         target.to_string_lossy());
                count += 1;
            }
        }
    }
}

fn explore_proc_system() {
    print_section("Part 2: 探索 /proc - 系統資訊");
    
    // CPU 資訊
    println!("{}CPU 資訊:{}", COLOR_GREEN, COLOR_RESET);
    if let Ok(content) = fs::read_to_string("/proc/cpuinfo") {
        let mut cpu_count = 0;
        let mut model_name = String::new();
        
        for line in content.lines() {
            if line.starts_with("processor") {
                cpu_count += 1;
            } else if cpu_count == 1 && line.starts_with("model name") {
                if let Some(pos) = line.find(':') {
                    model_name = line[pos+1..].trim().to_string();
                    print_item("CPU 型號", &model_name);
                }
            }
        }
        println!("  {}CPU 核心數: {}{}\n", COLOR_CYAN, cpu_count, COLOR_RESET);
    }
    
    // 記憶體資訊
    println!("{}記憶體資訊:{}", COLOR_GREEN, COLOR_RESET);
    if let Ok(content) = fs::read_to_string("/proc/meminfo") {
        for line in content.lines() {
            if line.starts_with("MemTotal:") ||
               line.starts_with("MemFree:") ||
               line.starts_with("MemAvailable:") {
                println!("  {}", line);
            }
        }
    }
    
    // 系統運行時間
    println!("\n{}系統運行時間:{}", COLOR_GREEN, COLOR_RESET);
    if let Ok(content) = fs::read_to_string("/proc/uptime") {
        let parts: Vec<&str> = content.split_whitespace().collect();
        if parts.len() >= 2 {
            if let (Ok(uptime), Ok(idle)) = (parts[0].parse::<f64>(), parts[1].parse::<f64>()) {
                let days = (uptime / 86400.0) as i32;
                let hours = ((uptime - days as f64 * 86400.0) / 3600.0) as i32;
                let minutes = ((uptime - days as f64 * 86400.0 - hours as f64 * 3600.0) / 60.0) as i32;
                println!("  已運行: {} 天 {} 小時 {} 分鐘", days, hours, minutes);
                println!("  閒置時間: {:.2} 秒", idle);
            }
        }
    }
}

// ============================================================================
// Part 2: 探索 /sys - 硬體與系統資訊
// ============================================================================

fn explore_sys() {
    print_section("Part 3: 探索 /sys - 硬體資訊");
    
    // 網路介面
    println!("{}網路介面:{}", COLOR_GREEN, COLOR_RESET);
    let net_class = Path::new("/sys/class/net");
    if let Ok(entries) = fs::read_dir(net_class) {
        for entry in entries.flatten() {
            let interface = entry.file_name();
            let interface_str = interface.to_string_lossy();
            
            println!("  {}介面: {}{}", COLOR_YELLOW, interface_str, COLOR_RESET);
            
            // 讀取 MAC 地址
            let mac_path = entry.path().join("address");
            if let Ok(mac) = fs::read_to_string(&mac_path) {
                println!("    MAC: {}", mac.trim());
            }
            
            // 讀取狀態
            let state_path = entry.path().join("operstate");
            if let Ok(state) = fs::read_to_string(&state_path) {
                println!("    狀態: {}", state.trim());
            }
            
            // 讀取 MTU
            let mtu_path = entry.path().join("mtu");
            if let Ok(mtu) = fs::read_to_string(&mtu_path) {
                println!("    MTU: {}", mtu.trim());
            }
            println!();
        }
    }
    
    // CPU 頻率（如果可用）
    println!("{}CPU 頻率資訊:{}", COLOR_GREEN, COLOR_RESET);
    let freq_path = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq";
    match fs::read_to_string(freq_path) {
        Ok(content) => {
            if let Ok(freq) = content.trim().parse::<f64>() {
                println!("  當前頻率: {:.2} GHz", freq / 1_000_000.0);
            }
        }
        Err(_) => {
            println!("  (無法讀取 CPU 頻率資訊，可能需要特定權限或不支援)");
        }
    }
}

// ============================================================================
// Part 3: 探索 /dev - 裝置檔案
// ============================================================================

fn explore_dev() {
    print_section("Part 4: 探索 /dev - 裝置檔案");
    
    let devices = [
        ("/dev/null", "黑洞裝置 (丟棄所有資料)"),
        ("/dev/zero", "零產生器 (產生無限的 0)"),
        ("/dev/random", "亂數產生器 (阻塞式)"),
        ("/dev/urandom", "亂數產生器 (非阻塞式)"),
        ("/dev/tty", "當前終端機"),
    ];
    
    println!("{}特殊裝置檔案:{}", COLOR_GREEN, COLOR_RESET);
    for (path, desc) in &devices {
        if let Ok(metadata) = fs::metadata(path) {
            use std::os::unix::fs::FileTypeExt;
            let file_type = metadata.file_type();
            let type_char = if file_type.is_block_device() {
                'b'
            } else if file_type.is_char_device() {
                'c'
            } else {
                '-'
            };
            println!("  {} {} - {}", type_char, path, desc);
        }
    }
    
    // 測試 /dev/urandom
    println!("\n{}測試 /dev/urandom (讀取 16 bytes):{}", COLOR_GREEN, COLOR_RESET);
    if let Ok(mut file) = fs::File::open("/dev/urandom") {
        let mut buffer = [0u8; 16];
        if file.read_exact(&mut buffer).is_ok() {
            print!("  亂數: ");
            for byte in &buffer {
                print!("{:02x} ", byte);
            }
            println!();
        }
    }
    
    // 列出區塊裝置
    println!("\n{}區塊裝置 (硬碟):{}", COLOR_GREEN, COLOR_RESET);
    if let Ok(entries) = fs::read_dir("/dev") {
        let mut count = 0;
        for entry in entries.flatten() {
            if count >= 10 { break; }
            let name = entry.file_name();
            let name_str = name.to_string_lossy();
            
            if name_str.starts_with("sd") || name_str.starts_with("nvme") {
                if let Ok(metadata) = entry.metadata() {
                    use std::os::unix::fs::FileTypeExt;
                    if metadata.file_type().is_block_device() {
                        use std::os::unix::fs::MetadataExt;
                        let rdev = metadata.rdev();
                        let major = (rdev >> 8) & 0xff;
                        let minor = rdev & 0xff;
                        println!("  /dev/{} (主要={}, 次要={})", name_str, major, minor);
                        count += 1;
                    }
                }
            }
        }
    }
}

// ============================================================================
// Main
// ============================================================================

fn main() {
    println!("\n{}", COLOR_BLUE);
    println!("═══════════════════════════════════════════════════════════════");
    println!("         探索 Linux 虛擬檔案系統 (/proc, /sys, /dev)          ");
    println!("═══════════════════════════════════════════════════════════════");
    print!("{}", COLOR_RESET);
    
    println!("\n{}重點:{}", COLOR_YELLOW, COLOR_RESET);
    println!("  • /proc 的檔案不存在硬碟上，是 Kernel 即時產生的");
    println!("  • /sys 提供硬體裝置的控制介面");
    println!("  • /dev 是硬體裝置的存取點");
    println!("  • 這三個都是「虛擬」檔案系統");
    
    explore_proc_current_process();
    explore_proc_system();
    explore_sys();
    explore_dev();
    
    print_section("總結");
    println!("✓ 透過標準的檔案 I/O 操作 (open, read, close)");
    println!("✓ 存取了 Kernel 空間的資訊");
    println!("✓ 不需要特殊的系統呼叫或 API");
    println!("✓ 這就是 Linux \"一切皆檔案\" 哲學的體現\n");
}
