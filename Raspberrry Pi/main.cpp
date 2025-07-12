#include "com.h"
#include <locale.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <thread>
#include <atomic>
#include <functional> 

using namespace std;

// 水质参数结构体
WaterQuality g_water_quality;
atomic<bool> g_running(true);

// 信号处理函数
void sigint_handler(int signum) {
    cout << "捕获Ctrl+C中断信号(SIGINT)，程序即将退出..." << endl;
    g_running = false;
}

#define BUFFER_SIZE 20

// 事件循环类
class EventLoop {
private:
    int epoll_fd;
    static const int MAX_EVENTS = 10;
    epoll_event events[MAX_EVENTS];
    
public:
    EventLoop() {
        epoll_fd = epoll_create1(0);
        if (epoll_fd == -1) {
            perror("epoll_create1");
            exit(EXIT_FAILURE);
        }
    }
    
    ~EventLoop() {
        close(epoll_fd);
    }
    
    void add_fd(int fd, function<void()> handler) {
        epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.ptr = new function<void()>(handler);
        
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
            perror("epoll_ctl: add");
            exit(EXIT_FAILURE);
        }
    }
    
    void run() {
        while (g_running) {
            int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
            if (nfds == -1) {
                if (errno == EINTR) continue;
                perror("epoll_wait");
                exit(EXIT_FAILURE);
            }
            
            for (int i = 0; i < nfds; ++i) {
                if (events[i].events & EPOLLIN) {
                    auto handler = static_cast<function<void()>*>(events[i].data.ptr);
                    (*handler)();
                }
            }
        }
    }
};

// 创建定时器fd
int create_timer_fd(int interval_ms) {
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
void read_timer_fd(int fd) {
    uint64_t expirations;
    ssize_t s = read(fd, &expirations, sizeof(expirations));
    if (s != sizeof(expirations)) {
        perror("read timerfd");
    }
}

// 调试信息更新函数
void update_debug_info() {
    cout << "调试信息更新" << endl;
    cout << "AIN0值 -> 浊度: " << g_water_quality.turbidity << endl;
    cout << "DS18B20值 -> 温度: " << g_water_quality.ds18b20 << "℃" << endl;
    cout << "pH值 -> pH: " << g_water_quality.pH << endl;
}

// TFT信息更新函数
void update_tft_info() {
    static TFTFreetype tft;
    static wchar_t turb[BUFFER_SIZE] = {0};
    
    memset(turb, ' ', BUFFER_SIZE);
    swprintf(turb, sizeof(turb) / sizeof(wchar_t), L"浊度: %.2f", g_water_quality.turbidity);
    tft.drawString(5, 20, turb,  0xFFFF);
    
    swprintf(turb, sizeof(turb) / sizeof(wchar_t), L"温度: %.2f℃", g_water_quality.ds18b20);
    tft.drawString(5, 50, turb,  0xFFFF);
    
    swprintf(turb, sizeof(turb) / sizeof(wchar_t), L"pH: %.2f", g_water_quality.pH);
    tft.drawString(5, 80, turb,  0xFFFF);
    
    tft.fillScreen(0x0000);
}

// 套接字通信函数
void update_socket_info(int sock) {
    char buf[128] = {0};
    sprintf(buf, "{\"tur\":\"%.2f\", \"tmp\":\"%.2f\", \"pH\":\"%.2f\"}", 
           g_water_quality.turbidity, g_water_quality.ds18b20, g_water_quality.pH);
    cout << buf << endl;
    
    send(sock, buf, strlen(buf), 0);
}

int main() {
    int sock;
    char serv_ip[] = "192.168.1.2";
    int port = 8888;
    Socket::connectToServer(&sock, serv_ip, port);
    cout << "连接成功: " << serv_ip << ":" << port << endl;

    // 初始化PCF8591
    PCF8591 pcf8591;
    cout << "PCF8591初始化成功!" << endl;

    // 初始化水质参数
    memset(&g_water_quality, 0, sizeof(g_water_quality));

    // 信号处理设置
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    cout << "程序正在运行，按Ctrl+C退出" << endl;

    // 创建事件循环
    EventLoop loop;
    
    // 创建数据采集定时器(1秒)
    int data_timer_fd = create_timer_fd(1000);
    loop.add_fd(data_timer_fd, [&]() {
        read_timer_fd(data_timer_fd);
        
        static DS18B20 ds18b20;
        static int channels[] = {0, 1};
        static int numChannels = 2;
        static int results[2];
        
        pcf8591.readMultiple(channels, numChannels, results);
        g_water_quality.turbidity = 100 - results[0] * 100.0 / 255;
        g_water_quality.ds18b20 = ds18b20.readTemperature();
        g_water_quality.pH = 14.0 - results[1] * 14.0/255.0;
    });
    
    // 创建调试信息定时器(1秒)
    int debug_timer_fd = create_timer_fd(1000);
    loop.add_fd(debug_timer_fd, [&]() {
        read_timer_fd(debug_timer_fd);
        update_debug_info();
    });
    
    // 创建TFT显示定时器(1秒)
    int tft_timer_fd = create_timer_fd(1000);
    loop.add_fd(tft_timer_fd, [&]() {
        read_timer_fd(tft_timer_fd);
        update_tft_info();
    });
    
    // 创建套接字通信定时器(1秒)
    int sock_timer_fd = create_timer_fd(1000);
    loop.add_fd(sock_timer_fd, [&]() {
        read_timer_fd(sock_timer_fd);
        update_socket_info(sock);
    });
    
    // 运行事件循环
    loop.run();
    
    // 清理资源
    close(data_timer_fd);
    close(debug_timer_fd);
    close(tft_timer_fd);
    close(sock_timer_fd);
    close(sock);

    return 0;
}
