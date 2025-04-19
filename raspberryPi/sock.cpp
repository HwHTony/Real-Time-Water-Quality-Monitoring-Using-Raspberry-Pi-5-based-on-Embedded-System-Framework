#include "sock.h"
#include <iostream>

void Socket::connect(int *sock, char* ip, int port) {
    struct sockaddr_in server_addr;
    int max_retries = 3;
    int retry_count = 0;

    while (retry_count < max_retries) {
        // 每次重试创建新套接字
        *sock = socket(AF_INET, SOCK_STREAM, 0);
        if (*sock < 0) {
            std::cerr << "套接字创建失败" << std::endl;
            exit(EXIT_FAILURE);
        }

        // 配置服务器地址
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
            std::cerr << "无效的服务器地址" << std::endl;
            close(*sock);
            retry_count++;
            std::cout << "第" << retry_count << "次尝试（地址转换失败）..." << std::endl;
            sleep(1);
            continue;
        }

        // 尝试连接
        if (connect(*sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "连接服务器失败" << std::endl;
            close(*sock);
            retry_count++;
            std::cout << "第" << retry_count << "次尝试（连接失败）..." << std::endl;
            sleep(1);
        } else {
            std::cout << "第" << retry_count + 1 << "次尝试成功！" << std::endl;
            return; // 连接成功，退出函数
        }
    }
}

void Socket::init(int *ser_sock, int *cli_sock) {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // 创建TCP套接字
    *ser_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*ser_sock < 0) {
        std::cerr << "套接字创建失败" << std::endl;
        exit(EXIT_FAILURE);
    }
    // 配置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // 监听所有网络接口
    server_addr.sin_port = htons(PORT);        // 端口号转网络字节序

    // 绑定套接字到指定地址和端口
    if (bind(*ser_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "绑定失败" << std::endl;
        close(*ser_sock);
        exit(-1);
    }

    // 开始监听（最大连接数5）
    if (listen(*ser_sock, 5) < 0) {
        std::cerr << "监听失败" << std::endl;
        close(*ser_sock);
        exit(EXIT_FAILURE);
    }

    std::cout << "服务器已启动，正在监听端口 " << PORT << "..." << std::endl;
}

void Socket::handleClient(int client_socket) {
    char buffer[BUFFER];
    ssize_t bytes_read;

    while ((bytes_read = recv(client_socket, buffer, BUFFER - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';  // 确保字符串终止
        std::cout << "收到数据: " << buffer << std::endl;

        // 回显数据给客户端
        send(client_socket, buffer, bytes_read, 0);
    }

    if (bytes_read == 0) {
        std::cout << "客户端断开连接" << std::endl;
    } else {
        std::cerr << "接收数据失败" << std::endl;
    }

    close(client_socket);
}