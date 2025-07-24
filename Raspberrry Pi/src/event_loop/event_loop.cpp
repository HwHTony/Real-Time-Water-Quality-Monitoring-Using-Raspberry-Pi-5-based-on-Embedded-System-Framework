#include "event_loop.h"
#include <iostream>
#include <cstring>
#include <unistd.h>  // Provides the close() function to close file descriptors

/**
 * @brief Event loop constructor, initialise epoll instance
 * @param run External atomic Boolean variable used to control loop start/stop
 * @throws If epoll_create1 fails, output an error message and terminate the program
 */
EventLoop::EventLoop(std::atomic<bool>& run) : running(run) {
    // Create an epoll instance (use epoll_create1(0) to automatically select the best mode)
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");  // Output system call error message
        std::exit(EXIT_FAILURE);  // Abnormal program termination
    }
}

/**
 * @brief Event loop destructor, clean up resources
 * @note Close the file descriptors of the epoll instance to prevent resource leaks
 */
EventLoop::~EventLoop() {
    close(epoll_fd);  // Close the epoll file descriptor
}

/**
 * @brief Add file descriptors and corresponding handling functions to the event loop
 * @param fd File descriptors to be monitored (such as timer fds, socket fds)
 * @param handler Callback function executed when an event is triggered
 * @throws If epoll_ctl fails, output an error message and terminate the program
 * @note When using edge-triggered mode (EPOLLET), ensure that the processing function reads/writes completely
 */
void EventLoop::add_fd(int fd, std::function<void()> handler) {
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;  // Set up a read event listener and use edge trigger mode
    // Store the processing function pointer in ev.data.ptr（Dynamic memory allocation required）
    ev.data.ptr = new std::function<void()>(handler);

    // Add file descriptors to the epoll instance
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        perror("epoll_ctl: add");  // Output failed addition information
        std::exit(EXIT_FAILURE);   // Abnormal program termination
    }
}

/**
 * @brief Start the event loop, begin listening for and processing events
 * @note The loop continues to run until the external setting running is set to false
 * @note Use epoll_wait to block and wait for events, supporting signal interrupts (EINTR)
 */
void EventLoop::run() {
    while (running) {  // Atomic variable control loop start/stop (thread-safe)
        // Waiting for an event to occur（-1 indicates infinite blocking until an event is triggered）
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            if (errno == EINTR)  // Handling signal interruptions
                continue;        // Ignore interrupts and continue waiting for events
            perror("epoll_wait");  // Output other error messages
            std::exit(EXIT_FAILURE);  // Abnormal program termination
        }

        // Iterate through all triggered events
        for (int i = 0; i < nfds; ++i) {
            if (events[i].events & EPOLLIN) {  // Check whether it is a read event
                // From data.ptrObtain the processing function pointer and call it
                auto handler = static_cast<std::function<void()>*>(events[i].data.ptr);
                (*handler)();  // Execute event handling function
            }
        }
    }
}
