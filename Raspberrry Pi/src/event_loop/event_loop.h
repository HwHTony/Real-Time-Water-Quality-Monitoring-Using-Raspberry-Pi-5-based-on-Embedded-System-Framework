// event_loop.h
#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H
/**
 * @file event_loop.h
 * @brief 基于epoll的事件循环类，用于高效处理多路I/O事件（如定时器、网络套接字等）
 * @note 线程安全设计，通过原子变量控制循环启停，支持动态添加文件描述符及事件处理函数
 */

#include <functional>   // 用于std::function（事件处理回调）
#include <sys/epoll.h>  // 用于epoll相关系统调用（epoll_create, epoll_ctl, epoll_wait等）
#include <atomic>       // 用于std::atomic<bool>（线程安全的循环状态控制）

/**
 * @class EventLoop
 * @brief 事件循环核心类，封装epoll机制实现I/O多路复用
 * 
 * 功能：管理多个文件描述符（如定时器fd、网络套接字fd），通过epoll_wait等待事件触发，
 * 并自动调用注册的回调函数处理事件。支持通过原子变量动态停止循环。
 */
class EventLoop {
private:
    int epoll_fd;  ///< epoll实例的文件描述符，通过epoll_create创建
    static const int MAX_EVENTS = 10;  ///< 单次epoll_wait最多处理的事件数
    epoll_event events[MAX_EVENTS];    ///< 存储epoll_wait返回的事件列表
    std::atomic<bool>& running;  ///< 原子布尔引用，控制事件循环是否运行（线程安全）

public:
    /**
     * @brief 构造函数，初始化epoll实例并绑定循环控制变量
     * @param run 原子布尔变量的引用，用于外部控制事件循环的启停
     */
    EventLoop(std::atomic<bool>& run);

    /**
     * @brief 析构函数，清理epoll资源
     * @note 关闭epoll实例的文件描述符，释放系统资源
     */
    ~EventLoop();

    /**
     * @brief 向事件循环添加文件描述符及对应的事件处理函数
     * @param fd 要监听的文件描述符（如定时器fd、套接字fd）
     * @param handler 事件触发时的回调函数（无参数，无返回值）
     * @note 默认为水平触发（EPOLLIN），可根据需求扩展事件类型
     */
    void add_fd(int fd, std::function<void()> handler);

    /**
     * @brief 启动事件循环，持续等待并处理事件
     * @note 循环逻辑：通过epoll_wait阻塞等待事件，遍历触发的事件并调用对应的回调函数，
     *       直到running变量被设为false时退出循环
     */
    void run();
};

#endif  // EVENT_LOOP_H