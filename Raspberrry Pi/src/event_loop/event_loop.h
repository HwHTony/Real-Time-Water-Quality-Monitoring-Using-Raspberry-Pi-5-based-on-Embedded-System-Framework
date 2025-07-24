// event_loop.h
#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H
/**
 * @file event_loop.h
 * @brief An event loop class based on epoll, used for efficient processing of multi-channel I/O events (such as timers, network sockets, etc.)
 * @note Thread-safe design, controlling loop start and stop through atomic variables, supporting dynamic addition of file descriptors and event handling functions
 */

#include <functional>   // Used for std::function (event handling callback)
#include <sys/epoll.h>  // Used for epoll-related system calls (epoll_create, epoll_ctl, epoll_wait, etc.)
#include <atomic>       // Used for std::atomic<bool> (thread-safe loop state control)

/**
 * @class EventLoop
 * @brief Event loop core class, encapsulating the epoll mechanism to implement I/O multiplexing
 * 
 * Function: Manage multiple file descriptors (such as timer fds and network socket fds) and wait for events to be triggered using epoll_wait,
 * And automatically call the registered callback function to handle events. Supports dynamically stopping the loop through atomic variables.
 */
class EventLoop {
private:
    int epoll_fd;  ///< The file descriptor of the epoll instance, created via epoll_create
    static const int MAX_EVENTS = 10;  ///< Maximum number of events processed by a single epoll_wait
    epoll_event events[MAX_EVENTS];    ///< Store the event list returned by epoll_wait
    std::atomic<bool>& running;  ///< Atomic Boolean reference, controls whether the event loop runs (thread-safe)

public:
    /**
     * @brief Constructor, initialise the epoll instance and bind the loop control variable
     * @param run Reference to an atomic Boolean variable used to start and stop the external control event loop
     */
    EventLoop(std::atomic<bool>& run);

    /**
     * @brief Destructor, clean up epoll resources
     * @note Close the file descriptor of the epoll instance and release system resources
     */
    ~EventLoop();

    /**
     * @brief Add file descriptors and corresponding event handling functions to the event loop
     * @param fd File descriptors to be monitored (such as timer fd, socket fd)
     * @param handler Callback function when the event is triggered (no parameters, no return value)
     * @note The default is horizontal triggering (EPOLLIN), and event types can be expanded as needed
     */
    void add_fd(int fd, std::function<void()> handler);

    /**
     * @brief Start the event loop, continuously wait for and process events
     * @note Loop logic: Block and wait for events using epoll_wait, iterate through the triggered events, and call the corresponding callback functions,
     *       Exit the loop when the running variable is set to false.
     */
    void run();
};

#endif  // EVENT_LOOP_H
