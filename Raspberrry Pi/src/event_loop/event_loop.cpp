#include "event_loop.h"
#include <iostream>
#include <cstring>
#include <unistd.h>  // 提供close()函数，用于关闭文件描述符

/**
 * @brief 事件循环构造函数，初始化epoll实例
 * @param run 外部传入的原子布尔变量，用于控制循环启停
 * @throws 若epoll_create1失败，输出错误信息并终止程序
 */
EventLoop::EventLoop(std::atomic<bool>& run) : running(run) {
    // 创建epoll实例（使用epoll_create1(0)自动选择最佳模式）
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");  // 输出系统调用错误信息
        std::exit(EXIT_FAILURE);  // 异常退出程序
    }
}

/**
 * @brief 事件循环析构函数，清理资源
 * @note 关闭epoll实例的文件描述符，防止资源泄漏
 */
EventLoop::~EventLoop() {
    close(epoll_fd);  // 关闭epoll文件描述符
}

/**
 * @brief 向事件循环添加文件描述符及对应处理函数
 * @param fd 待监听的文件描述符（如定时器fd、套接字fd）
 * @param handler 事件触发时执行的回调函数
 * @throws 若epoll_ctl失败，输出错误信息并终止程序
 * @note 使用边缘触发模式(EPOLLET)，需确保处理函数读取/写入完全
 */
void EventLoop::add_fd(int fd, std::function<void()> handler) {
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;  // 设置监听读事件并使用边缘触发模式
    // 将处理函数指针存入ev.data.ptr（需动态分配内存）
    ev.data.ptr = new std::function<void()>(handler);

    // 将文件描述符添加到epoll实例中
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        perror("epoll_ctl: add");  // 输出添加失败信息
        std::exit(EXIT_FAILURE);   // 异常退出程序
    }
}

/**
 * @brief 启动事件循环，开始监听并处理事件
 * @note 循环持续运行直到外部设置running为false
 * @note 使用epoll_wait阻塞等待事件，支持处理信号中断(EINTR)
 */
void EventLoop::run() {
    while (running) {  // 原子变量控制循环启停（线程安全）
        // 等待事件发生（-1表示无限阻塞，直到有事件触发）
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            if (errno == EINTR)  // 处理被信号中断的情况
                continue;        // 忽略中断，继续等待事件
            perror("epoll_wait");  // 输出其他错误信息
            std::exit(EXIT_FAILURE);  // 异常退出程序
        }

        // 遍历所有触发的事件
        for (int i = 0; i < nfds; ++i) {
            if (events[i].events & EPOLLIN) {  // 检查是否为读事件
                // 从data.ptr中获取处理函数指针并调用
                auto handler = static_cast<std::function<void()>*>(events[i].data.ptr);
                (*handler)();  // 执行事件处理函数
            }
        }
    }
}