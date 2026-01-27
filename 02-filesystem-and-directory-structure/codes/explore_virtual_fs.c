/**
 * explore_virtual_fs.c - 深入探索 Linux 虛擬檔案系統
 * 
 * 本程式示範如何透過程式存取 /proc, /sys, /dev 這三個虛擬檔案系統
 * 
 * 編譯: gcc -o explore_virtual_fs explore_virtual_fs.c
 * 執行: ./explore_virtual_fs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include <dirent.h>

#define COLOR_RESET   "\033[0m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_CYAN    "\033[36m"

void print_section(const char *title) {
    printf("\n%s╔═══════════════════════════════════════════════════════════╗%s\n", 
           COLOR_BLUE, COLOR_RESET);
    printf("%s║ %-57s ║%s\n", COLOR_BLUE, title, COLOR_RESET);
    printf("%s╚═══════════════════════════════════════════════════════════╝%s\n\n", 
           COLOR_BLUE, COLOR_RESET);
}

void print_item(const char *label, const char *value) {
    printf("%s%-20s:%s %s\n", COLOR_CYAN, label, COLOR_RESET, value);
}

// ============================================================================
// Part 1: 探索 /proc - 程序與核心資訊
// ============================================================================

void explore_proc_current_process() {
    print_section("Part 1: 探索 /proc - 當前程序資訊");
    
    pid_t pid = getpid();
    pid_t ppid = getppid();
    
    printf("當前程序資訊：\n");
    printf("  PID:  %d\n", pid);
    printf("  PPID: %d\n\n", ppid);
    
    // 讀取執行檔路徑
    char exe_path[1024];
    char proc_path[256];
    snprintf(proc_path, sizeof(proc_path), "/proc/%d/exe", pid);
    
    ssize_t len = readlink(proc_path, exe_path, sizeof(exe_path) - 1);
    if (len != -1) {
        exe_path[len] = '\0';
        print_item("執行檔路徑", exe_path);
    }
    
    // 讀取命令列
    snprintf(proc_path, sizeof(proc_path), "/proc/%d/cmdline", pid);
    FILE *f = fopen(proc_path, "r");
    if (f) {
        char cmdline[256] = {0};
        fread(cmdline, 1, sizeof(cmdline) - 1, f);
        // 將 null bytes 轉換為空格
        for (int i = 0; cmdline[i]; i++) {
            if (cmdline[i] == '\0' && cmdline[i+1]) cmdline[i] = ' ';
        }
        print_item("命令列", cmdline);
        fclose(f);
    }
    
    // 讀取狀態
    snprintf(proc_path, sizeof(proc_path), "/proc/%d/status", pid);
    f = fopen(proc_path, "r");
    if (f) {
        printf("\n%s重要狀態資訊:%s\n", COLOR_GREEN, COLOR_RESET);
        char line[256];
        while (fgets(line, sizeof(line), f)) {
            if (strncmp(line, "State:", 6) == 0 ||
                strncmp(line, "Threads:", 8) == 0 ||
                strncmp(line, "VmSize:", 7) == 0 ||
                strncmp(line, "VmRSS:", 6) == 0 ||
                strncmp(line, "VmData:", 7) == 0) {
                printf("  %s", line);
            }
        }
        fclose(f);
    }
    
    // 列出開啟的檔案描述符
    printf("\n%s開啟的檔案描述符 (前 5 個):%s\n", COLOR_GREEN, COLOR_RESET);
    snprintf(proc_path, sizeof(proc_path), "/proc/%d/fd", pid);
    DIR *dir = opendir(proc_path);
    if (dir) {
        struct dirent *entry;
        int count = 0;
        while ((entry = readdir(dir)) != NULL && count < 5) {
            if (entry->d_name[0] == '.') continue;
            
            char fd_path[512], link_target[512];
            snprintf(fd_path, sizeof(fd_path), "/proc/%d/fd/%s", pid, entry->d_name);
            len = readlink(fd_path, link_target, sizeof(link_target) - 1);
            if (len != -1) {
                link_target[len] = '\0';
                printf("  fd %s -> %s\n", entry->d_name, link_target);
                count++;
            }
        }
        closedir(dir);
    }
}

void explore_proc_system() {
    print_section("Part 2: 探索 /proc - 系統資訊");
    
    // CPU 資訊
    printf("%sCPU 資訊:%s\n", COLOR_GREEN, COLOR_RESET);
    FILE *f = fopen("/proc/cpuinfo", "r");
    if (f) {
        char line[256];
        int cpu_count = 0;
        while (fgets(line, sizeof(line), f)) {
            if (strncmp(line, "processor", 9) == 0) {
                cpu_count++;
            } else if (cpu_count == 1 && strncmp(line, "model name", 10) == 0) {
                char *model = strchr(line, ':');
                if (model) {
                    model += 2; // 跳過 ": "
                    print_item("CPU 型號", model);
                }
            }
        }
        printf("  %sCPU 核心數: %d%s\n\n", COLOR_CYAN, cpu_count, COLOR_RESET);
        fclose(f);
    }
    
    // 記憶體資訊
    printf("%s記憶體資訊:%s\n", COLOR_GREEN, COLOR_RESET);
    f = fopen("/proc/meminfo", "r");
    if (f) {
        char line[256];
        while (fgets(line, sizeof(line), f)) {
            if (strncmp(line, "MemTotal:", 9) == 0 ||
                strncmp(line, "MemFree:", 8) == 0 ||
                strncmp(line, "MemAvailable:", 13) == 0) {
                printf("  %s", line);
            }
        }
        fclose(f);
    }
    
    // 系統運行時間
    printf("\n%s系統運行時間:%s\n", COLOR_GREEN, COLOR_RESET);
    f = fopen("/proc/uptime", "r");
    if (f) {
        double uptime, idle;
        if (fscanf(f, "%lf %lf", &uptime, &idle) == 2) {
            int days = (int)(uptime / 86400);
            int hours = (int)((uptime - days * 86400) / 3600);
            int minutes = (int)((uptime - days * 86400 - hours * 3600) / 60);
            printf("  已運行: %d 天 %d 小時 %d 分鐘\n", days, hours, minutes);
            printf("  閒置時間: %.2f 秒\n", idle);
        }
        fclose(f);
    }
}

// ============================================================================
// Part 2: 探索 /sys - 硬體與系統資訊
// ============================================================================

void explore_sys() {
    print_section("Part 3: 探索 /sys - 硬體資訊");
    
    // 網路介面
    printf("%s網路介面:%s\n", COLOR_GREEN, COLOR_RESET);
    DIR *dir = opendir("/sys/class/net");
    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] == '.') continue;
            
            printf("  %s介面: %s%s\n", COLOR_YELLOW, entry->d_name, COLOR_RESET);
            
            // 讀取 MAC 地址
            char path[256], buffer[128];
            snprintf(path, sizeof(path), "/sys/class/net/%s/address", entry->d_name);
            FILE *f = fopen(path, "r");
            if (f) {
                if (fgets(buffer, sizeof(buffer), f)) {
                    buffer[strcspn(buffer, "\n")] = 0;
                    printf("    MAC: %s\n", buffer);
                }
                fclose(f);
            }
            
            // 讀取狀態
            snprintf(path, sizeof(path), "/sys/class/net/%s/operstate", entry->d_name);
            f = fopen(path, "r");
            if (f) {
                if (fgets(buffer, sizeof(buffer), f)) {
                    buffer[strcspn(buffer, "\n")] = 0;
                    printf("    狀態: %s\n", buffer);
                }
                fclose(f);
            }
            
            // 讀取 MTU
            snprintf(path, sizeof(path), "/sys/class/net/%s/mtu", entry->d_name);
            f = fopen(path, "r");
            if (f) {
                if (fgets(buffer, sizeof(buffer), f)) {
                    buffer[strcspn(buffer, "\n")] = 0;
                    printf("    MTU: %s\n", buffer);
                }
                fclose(f);
            }
            printf("\n");
        }
        closedir(dir);
    }
    
    // CPU 頻率（如果可用）
    printf("%sCPU 頻率資訊:%s\n", COLOR_GREEN, COLOR_RESET);
    FILE *f = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", "r");
    if (f) {
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), f)) {
            long freq = atol(buffer);
            printf("  當前頻率: %.2f GHz\n", freq / 1000000.0);
        }
        fclose(f);
    } else {
        printf("  (無法讀取 CPU 頻率資訊，可能需要特定權限或不支援)\n");
    }
}

// ============================================================================
// Part 3: 探索 /dev - 裝置檔案
// ============================================================================

void explore_dev() {
    print_section("Part 4: 探索 /dev - 裝置檔案");
    
    struct {
        const char *path;
        const char *description;
    } devices[] = {
        {"/dev/null", "黑洞裝置 (丟棄所有資料)"},
        {"/dev/zero", "零產生器 (產生無限的 0)"},
        {"/dev/random", "亂數產生器 (阻塞式)"},
        {"/dev/urandom", "亂數產生器 (非阻塞式)"},
        {"/dev/tty", "當前終端機"},
        {NULL, NULL}
    };
    
    printf("%s特殊裝置檔案:%s\n", COLOR_GREEN, COLOR_RESET);
    for (int i = 0; devices[i].path; i++) {
        struct stat st;
        if (stat(devices[i].path, &st) == 0) {
            char type = '-';
            if (S_ISBLK(st.st_mode)) type = 'b';
            else if (S_ISCHR(st.st_mode)) type = 'c';
            
            printf("  %c %s - %s\n", type, devices[i].path, devices[i].description);
        }
    }
    
    // 測試 /dev/urandom
    printf("\n%s測試 /dev/urandom (讀取 16 bytes):%s\n", COLOR_GREEN, COLOR_RESET);
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd >= 0) {
        unsigned char buffer[16];
        ssize_t n = read(fd, buffer, sizeof(buffer));
        if (n > 0) {
            printf("  亂數: ");
            for (int i = 0; i < n; i++) {
                printf("%02x ", buffer[i]);
            }
            printf("\n");
        }
        close(fd);
    }
    
    // 列出區塊裝置
    printf("\n%s區塊裝置 (硬碟):%s\n", COLOR_GREEN, COLOR_RESET);
    DIR *dir = opendir("/dev");
    if (dir) {
        struct dirent *entry;
        int count = 0;
        while ((entry = readdir(dir)) != NULL && count < 10) {
            if (strncmp(entry->d_name, "sd", 2) == 0 || 
                strncmp(entry->d_name, "nvme", 4) == 0) {
                char path[256];
                snprintf(path, sizeof(path), "/dev/%s", entry->d_name);
                struct stat st;
                if (stat(path, &st) == 0 && S_ISBLK(st.st_mode)) {
                    printf("  /dev/%s (主要=%d, 次要=%d)\n", 
                           entry->d_name, major(st.st_rdev), minor(st.st_rdev));
                    count++;
                }
            }
        }
        closedir(dir);
    }
}

// ============================================================================
// Main
// ============================================================================

int main() {
    printf("\n%s", COLOR_BLUE);
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("         探索 Linux 虛擬檔案系統 (/proc, /sys, /dev)          \n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("%s", COLOR_RESET);
    
    printf("\n%s重點:%s\n", COLOR_YELLOW, COLOR_RESET);
    printf("  • /proc 的檔案不存在硬碟上，是 Kernel 即時產生的\n");
    printf("  • /sys 提供硬體裝置的控制介面\n");
    printf("  • /dev 是硬體裝置的存取點\n");
    printf("  • 這三個都是「虛擬」檔案系統\n");
    
    explore_proc_current_process();
    explore_proc_system();
    explore_sys();
    explore_dev();
    
    print_section("總結");
    printf("✓ 透過標準的檔案 I/O 操作 (open, read, close)\n");
    printf("✓ 存取了 Kernel 空間的資訊\n");
    printf("✓ 不需要特殊的系統呼叫或 API\n");
    printf("✓ 這就是 Linux \"一切皆檔案\" 哲學的體現\n\n");
    
    return 0;
}
