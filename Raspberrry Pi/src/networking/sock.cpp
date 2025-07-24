#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>

#include "sock.h"

/**
 * @brief 连接到TCP服务器（带重试机制）
 * @param sock [输出] 成功连接后返回的套接字描述符
 * @param ip 服务器IP地址字符串（如"192.168.1.2"）
 * @param port 服务器端口号
 * @return 0表示成功，-1表示失败
 * @note 内部实现3次重试，失败时通过日志输出错误信息
 */
int Socket::connectToServer(int *sock, const char* ip, int port) {
    if (sock == nullptr || ip == nullptr || port < 1 || port > 65535) {
        std::cerr << "错误：无效的参数 - sock不可为空，IP不可为空，端口需在1-65535范围内" << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr;
    const int max_retries = 3;       // 最大重试次数
    int retry_count = 0;

    // 循环重试连接
    while (retry_count < max_retries) {
        // 1. 创建TCP套接字（IPv4协议，字节流）
        *sock = socket(AF_INET, SOCK_STREAM, 0);
        if (*sock < 0) {
            std::cerr << "错误：套接字创建失败 - " << strerror(errno) << std::endl;
            return -1;
        }

        // 2. 配置服务器地址结构
        memset(&server_addr, 0, sizeof(server_addr));  // 初始化地址结构体
        server_addr.sin_family = AF_INET;              // IPv4协议
        server_addr.sin_port = htons(port);             // 端口号转为网络字节序（大端）

        // 转换IP地址（字符串转网络字节序）
        if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
            close(*sock);  // 释放当前无效套接字
            retry_count++;
            std::cerr << "第" << retry_count << "次尝试失败：无效的IP地址（" << ip << "）" << std::endl;
            sleep(1);  // 重试前等待1秒
            continue;
        }

        // 3. 尝试连接服务器
        if (connect(*sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            close(*sock);  // 释放当前无效套接字
            retry_count++;
            std::cerr << "第" << retry_count << "次尝试失败：连接服务器（" << ip << ":" << port << "）失败：" << strerror(errno) << std::endl;
            sleep(1);  // 重试前等待1秒
        } else {
            // 连接成功
            std::cout << "成功连接到服务器：" << ip << ":" << port << std::endl;
            return 0; // 成功返回0
        }
    }

    // 超过最大重试次数仍失败
    std::cerr << "错误：超过最大重试次数（" << max_retries << "次），连接服务器失败" << std::endl;
    return -1;
}


/**
 * @brief 初始化TCP服务器（创建监听套接字并绑定端口）
 * @param ser_sock [输出] 服务器监听套接字描述符
 * @param cli_sock [输出] 客户端连接套接字描述符（预留，实际应在accept时获取）
 * @note 该函数仅完成服务器初始化，需在外部调用accept获取客户端连接
 */
void Socket::init(int *ser_sock, int *cli_sock) {
    if (ser_sock == nullptr || cli_sock == nullptr) {
        throw std::invalid_argument("无效的参数：ser_sock和cli_sock不可为空");
    }

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // 1. 创建TCP监听套接字
    *ser_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*ser_sock < 0) {
        throw std::runtime_error("监听套接字创建失败：" + std::string(strerror(errno)));
    }

    // 2. 设置套接字选项（允许端口复用，避免服务器重启时端口占用）
    int opt = 1;
    if (setsockopt(*ser_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(*ser_sock);
        throw std::runtime_error("设置套接字选项失败：" + std::string(strerror(errno)));
    }

    // 3. 配置服务器地址结构
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;                  // IPv4协议
    server_addr.sin_addr.s_addr = INADDR_ANY;          // 监听所有可用网络接口（0.0.0.0）
    server_addr.sin_port = htons(PORT);                // 端口号转为网络字节序

    // 4. 绑定套接字到指定地址和端口
    if (bind(*ser_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(*ser_sock);
        throw std::runtime_error("绑定端口（" + std::to_string(PORT) + "）失败：" + std::string(strerror(errno)));
    }

    // 5. 开始监听（最大等待连接队列长度为5）
    if (listen(*ser_sock, 5) < 0) {
        close(*ser_sock);
        throw std::runtime_error("监听端口（" + std::to_string(PORT) + "）失败：" + std::string(strerror(errno)));
    }

    // 初始化客户端套接字（默认-1表示未连接）
    *cli_sock = -1;

    std::cout << "服务器初始化完成，正在端口 " << PORT << " 监听连接..." << std::endl;
}


/**
 * @brief 处理客户端连接（接收数据并回显）
 * @param client_socket 客户端套接字描述符
 * @note 建议在独立线程中调用，支持并发处理多个客户端
 */
void Socket::handleClient(int client_socket) {
    if (client_socket < 0) {
        throw std::invalid_argument("无效的客户端套接字：" + std::to_string(client_socket));
    }

    char buffer[BUFFER];  // 数据缓冲区
    ssize_t bytes_read;   // 接收的字节数

    std::cout << "开始处理客户端（套接字：" << client_socket << "）的数据..." << std::endl;

    // 循环接收客户端数据
    while ((bytes_read = recv(client_socket, buffer, BUFFER - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';  // 确保字符串以null结尾
        std::cout << "收到客户端（" << client_socket << "）数据：" << buffer << std::endl;

        // 回显数据给客户端（可扩展为业务逻辑处理）
        if (send(client_socket, buffer, bytes_read, 0) < 0) {
            std::cerr << "向客户端（" << client_socket << "）发送数据失败：" << strerror(errno) << std::endl;
            break;
        }
    }

    // 处理连接结束或错误
    if (bytes_read == 0) {
        std::cout << "客户端（" << client_socket << "）主动断开连接" << std::endl;
    } else if (bytes_read < 0) {
        std::cerr << "接收客户端（" << client_socket << "）数据时发生错误：" << strerror(errno) << std::endl;
    }

    // 关闭客户端套接字，释放资源
    close(client_socket);
    std::cout << "客户端（" << client_socket << "）连接已关闭" << std::endl;
}