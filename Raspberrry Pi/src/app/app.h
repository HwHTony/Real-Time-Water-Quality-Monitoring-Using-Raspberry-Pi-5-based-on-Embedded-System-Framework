// app.h
#ifndef APP_H
#define APP_H

#include <atomic>
#include <functional>
#include <vector>
#include "../event_loop/event_loop.h"
#include "../data_collection/data_collector.h"
#include "../info_updating/info_updater.h"
#include "../networking/sock.h"
#include <signal.h> // 添加 <signal.h> 头文件

class App {
private:
    std::atomic<bool> running;
    EventLoop loop;
    int sock;
    std::vector<InfoUpdater*> updaters;
    std::vector<int> timer_fds;

    // 信号处理函数
    static void sigint_handler(int signum, siginfo_t *info, void *context);

    // 创建定时器fd
    int create_timer_fd(int interval_ms);

    // 读取定时器事件
    void read_timer_fd(int fd);

public:
    App();
    void init();
    void run();
    void cleanup();
};

#endif