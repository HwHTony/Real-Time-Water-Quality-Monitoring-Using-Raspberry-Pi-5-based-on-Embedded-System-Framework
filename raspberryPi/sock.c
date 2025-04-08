#include "sock.h"

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';  // 确保字符串终止
        printf("收到数据: %s", buffer);

        // 回显数据给客户端
        send(client_socket, buffer, bytes_read, 0);
    }

    if (bytes_read == 0) {
        printf("客户端断开连接");
    } else {
        perror("接收数据失败");
    }

    close(client_socket);
}

void sock_init(int *ser_sock, int *cli_sock)
{
    
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // 创建TCP套接字
    *ser_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*ser_sock < 0) {
        perror("套接字创建失败");
        exit(EXIT_FAILURE);
    }
    // 配置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // 监听所有网络接口
    server_addr.sin_port = htons(PORT);        // 端口号转网络字节序

    // 绑定套接字到指定地址和端口
    if (bind(*ser_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("绑定失败");
        close(*ser_sock);
        exit(-1);
    }

    // 开始监听（最大连接数5）
    if (listen(*ser_sock, 5) < 0) {
        perror("监听失败");
        close(*ser_sock);
        exit(EXIT_FAILURE);
    }

    printf("服务器已启动，正在监听端口 %d...", PORT);
}

void serv_connect(int *sock, char* ip, int port)
{
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock < 0) {
        perror("套接字创建失败");
        exit(-1);
    }

    struct sockaddr_in server_addr, client_addr;
    // 配置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("无效的服务器地址");
        close(*sock);
        exit(-1);
    }

    int count = 1;
    while (count <= 3)
    {
        // 连接服务器
        if (connect(*sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("连接服务器失败");
            if (3 == count)
            {
                close(*sock);
                printf("请检查服务器状态\n");
                exit(-1);
            }
        }
        sleep(1);
        printf("第%d次尝试连接", ++count);
    }

}