#include "app.h"
#include <iostream>
#include <cstring>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <signal.h>
#include <cstdlib>
#include <sstream>   // New: For std::stringstream
#include <algorithm> // New: Used for std::all_of
#include <fstream>   // New: For std::ifstream
#include <vector>    // Ensure that the vector header file is included.
#include "../info_updating/debug_info_updater.h"
#include "../info_updating/tft_info_updater.h"
#include "../info_updating/socket_info_updater.h"

App::App() : running(true), loop(running) {}

// signal processing function
void App::sigint_handler(int signum, siginfo_t *info, void *context) {
    App* app = static_cast<App*>(context);
    std::cout << ‘Catching Ctrl+C interrupt signal (SIGINT), program about to exit...’ << std::endl;
    app->running = false;
}

// Create timer fd
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

// Read timer event
void App::read_timer_fd(int fd) {
    uint64_t expirations;
    ssize_t s = read(fd, &expirations, sizeof(expirations));
    if (s != sizeof(expirations)) {
        perror("read timerfd");
    }
}

// Trim leading and trailing spaces from strings
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

// Verify that the IP address is valid and within the range of 192.168.1.1-192.168.1.100.
bool isValidIpAddress(const std::string& ip) {
    std::vector<std::string> parts;
    std::stringstream ss(ip);  // Already includes <sstream>, can be used normally.
    std::string part;
    
    // Split IP addresses with '.'
    while (std::getline(ss, part, '.')) {
        parts.push_back(part);
    }
    
    // It must be 4 paragraphs
    if (parts.size() != 4) {
        return false;
    }
    
    // Check the validity of each segment
    int segments[4];
    for (int i = 0; i < 4; i++) {
        // The segment is empty or contains non-numeric characters.
        if (parts[i].empty() || !std::all_of(parts[i].begin(), parts[i].end(), ::isdigit)) {  // Included<algorithm>
            return false;
        }
        
        // Convert to an integer and check if it is within the range of 0-255
        try {
            segments[i] = std::stoi(parts[i]);
        } catch (...) {
            return false;
        }
        if (segments[i] < 0 || segments[i] > 255) {
            return false;
        }
        
        // Check the leading zero (e.g., ‘012’ is invalid, ‘0’ is valid)
        if (parts[i].size() > 1 && parts[i][0] == '0') {
            return false;
        }
    }
    
    // Check whether it is within the range of 192.168.1.1-192.168.1.100.
    if (segments[0] != 192 || segments[1] != 168 || segments[2] != 1) {
        return false;
    }
    if (segments[3] < 1 || segments[3] > 100) {
        return false;
    }
    
    return true;
}

void App::init() {
    // 1. Read IP address from file (includes <fstream>, can be used normally)
    std::string ip;
    std::ifstream file("config.txt");
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open configuration file config.txt" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Read the first line and remove leading and trailing spaces
    std::getline(file, ip);
    ip = trim(ip);
    file.close();
    
    // 2. Verify IP address validity and range
    if (ip.empty()) {
        std::cerr << "Error: IP address is empty in the configuration file" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (!isValidIpAddress(ip)) {
        std::cerr << "Error: IP address " << ip << " Illegal or not within the range of 192.168.1.1-192.168.1.100" << std::endl;
        exit(EXIT_FAILURE);
    }

    // 3. Connect to server (fix socket function parameters and return values)
    int port = 8888;
    // Socket::connectToServer Modified to return int，The parameter is const char*
    if (Socket::connectToServer(&sock, ip.c_str(), port) != 0) {
        std::cerr << "Error: Connecting to server " << ip << ":" << port << " failure" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Connection successful: " << ip << ":" << port << std::endl;

    // Signal processing settings
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sigint_handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    std::cout << "The program is running，The program is running. Press Ctrl+C log out" << std::endl;

    // Data acquisition timer
    DataCollector dataCollector;
    int data_timer_fd = create_timer_fd(1000);
    loop.add_fd(data_timer_fd, [this, &dataCollector, data_timer_fd]() {
        read_timer_fd(data_timer_fd);
        dataCollector.collectData();
    });
    timer_fds.push_back(data_timer_fd);

    // Debugging information timer
    DebugInfoUpdater debugInfoUpdater;
    int debug_timer_fd = create_timer_fd(1000);
    loop.add_fd(debug_timer_fd, [this, &debugInfoUpdater, debug_timer_fd]() {
        read_timer_fd(debug_timer_fd);
        debugInfoUpdater.update();
    });
    updaters.push_back(&debugInfoUpdater);
    timer_fds.push_back(debug_timer_fd);

    // TFT display timer
    TFTInfoUpdater tftInfoUpdater;
    int tft_timer_fd = create_timer_fd(1000);
    loop.add_fd(tft_timer_fd, [this, &tftInfoUpdater, tft_timer_fd]() {
        read_timer_fd(tft_timer_fd);
        tftInfoUpdater.update();
    });
    updaters.push_back(&tftInfoUpdater);
    timer_fds.push_back(tft_timer_fd);

    // Socket communication timer
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
