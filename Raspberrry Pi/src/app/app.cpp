#include "app.h"
#include <iostream>
#include <cstring>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <signal.h>
#include <cstdlib>
#include <sstream>   // 新增：用于std::stringstream
#include <algorithm> // 新增：用于std::all_of
#include <fstream>   // 新增：用于std::ifstream
#include <vector>    // 确保vector头文件包含
#include "../info_updating/debug_info_updater.h"
#include "../info_updating/tft_info_updater.h"
#include "../info_updating/socket_info_updater.h"

App::App() : running(true), loop(running) {}

// 信号处理函数
void App::sigint_handler(int signum, siginfo_t *info, void *context) {
    App* app = static_cast<App*>(context);
    std::cout << "捕获Ctrl+C中断信号(SIGINT)，程序即将退出..." << std::endl;
    app->running = false;
}

// 创建定时器fd
int App::create_timer_fd(int interval_ms) {
    int fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (fd == -1) {
        perror("timerfd_create");
        exit(EXIT_FAILURE);
    }

    itimerspec its;
    its.it_interval.tv_sec = interval_ms / 1000;
    its.it_interval.tv_nsec = (interval_ms % 1000) * 1000000;
    its.it_value = its.it_interval;

    if (timerfd_settime(fd, 0, &its, NULL) == -1) {
        perror("timerfd_settime");
        exit(EXIT_FAILURE);
    }

    return fd;
}

// 读取定时器事件
void App::read_timer_fd(int fd) {
    uint64_t expirations;
    ssize_t s = read(fd, &expirations, sizeof(expirations));
    if (s != sizeof(expirations)) {
        perror("read timerfd");
    }
}

// 修剪字符串首尾空格
std::string trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start)) {
        start++;
    }
    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));
    return std::string(start, end + 1);
}

// 校验IP地址是否合法且在192.168.1.1-192.168.1.100范围内
bool isValidIpAddress(const std::string& ip) {
    std::vector<std::string> parts;
    std::stringstream ss(ip);  // 已包含<sstream>，可正常使用
    std::string part;
    
    // 按 '.' 分割IP地址
    while (std::getline(ss, part, '.')) {
        parts.push_back(part);
    }
    
    // 必须是4个段
    if (parts.size() != 4) {
        return false;
    }
    
    // 检查每个段的合法性
    int segments[4];
    for (int i = 0; i < 4; i++) {
        // 段为空或包含非数字字符
        if (parts[i].empty() || !std::all_of(parts[i].begin(), parts[i].end(), ::isdigit)) {  // 已包含<algorithm>
            return false;
        }
        
        // 转换为整数，检查是否在0-255范围内
        try {
            segments[i] = std::stoi(parts[i]);
        } catch (...) {
            return false;
        }
        if (segments[i] < 0 || segments[i] > 255) {
            return false;
        }
        
        // 检查前导零（如 "012" 不合法，"0" 合法）
        if (parts[i].size() > 1 && parts[i][0] == '0') {
            return false;
        }
    }
    
    // 检查是否在192.168.1.1-192.168.1.100范围内
    if (segments[0] != 192 || segments[1] != 168 || segments[2] != 1) {
        return false;
    }
    if (segments[3] < 1 || segments[3] > 100) {
        return false;
    }
    
    return true;
}

void App::init() {
    // 1. 从文件读取IP地址（已包含<fstream>，可正常使用）
    std::string ip;
    std::ifstream file("config.txt");
    if (!file.is_open()) {
        std::cerr << "错误：无法打开配置文件 config.txt" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // 读取第一行并去除首尾空格
    std::getline(file, ip);
    ip = trim(ip);
    file.close();
    
    // 2. 校验IP地址合法性及范围
    if (ip.empty()) {
        std::cerr << "错误：配置文件中IP地址为空" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (!isValidIpAddress(ip)) {
        std::cerr << "错误：IP地址 " << ip << " 不合法或不在192.168.1.1-192.168.1.100范围内" << std::endl;
        exit(EXIT_FAILURE);
    }

    // 3. 连接服务器（修复Socket函数参数和返回值）
    int port = 8888;
    // Socket::connectToServer已修改为返回int，参数为const char*
    if (Socket::connectToServer(&sock, ip.c_str(), port) != 0) {
        std::cerr << "错误：连接服务器 " << ip << ":" << port << " 失败" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "连接成功: " << ip << ":" << port << std::endl;

    // 信号处理设置
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sigint_handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    std::cout << "程序正在运行，按Ctrl+C退出" << std::endl;

    // 数据采集定时器
    DataCollector dataCollector;
    int data_timer_fd = create_timer_fd(1000);
    loop.add_fd(data_timer_fd, [this, &dataCollector, data_timer_fd]() {
        read_timer_fd(data_timer_fd);
        dataCollector.collectData();
    });
    timer_fds.push_back(data_timer_fd);

    // 调试信息定时器
    DebugInfoUpdater debugInfoUpdater;
    int debug_timer_fd = create_timer_fd(1000);
    loop.add_fd(debug_timer_fd, [this, &debugInfoUpdater, debug_timer_fd]() {
        read_timer_fd(debug_timer_fd);
        debugInfoUpdater.update();
    });
    updaters.push_back(&debugInfoUpdater);
    timer_fds.push_back(debug_timer_fd);

    // TFT显示定时器
    TFTInfoUpdater tftInfoUpdater;
    int tft_timer_fd = create_timer_fd(1000);
    loop.add_fd(tft_timer_fd, [this, &tftInfoUpdater, tft_timer_fd]() {
        read_timer_fd(tft_timer_fd);
        tftInfoUpdater.update();
    });
    updaters.push_back(&tftInfoUpdater);
    timer_fds.push_back(tft_timer_fd);

    // 套接字通信定时器
    SocketInfoUpdater socketInfoUpdater(sock);
    int sock_timer_fd = create_timer_fd(1000);
    loop.add_fd(sock_timer_fd, [this, &socketInfoUpdater, sock_timer_fd]() {
        read_timer_fd(sock_timer_fd);
        socketInfoUpdater.update();
    });
    updaters.push_back(&socketInfoUpdater);
    timer_fds.push_back(sock_timer_fd);
}

void App::run() {
    loop.run();
}

void App::cleanup() {
    for (int fd : timer_fds) {
        close(fd);
    }
    close(sock);
}