#ifndef SOCK_H
#define SOCK_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../common/com.h"

#define PORT 8888
#define BUFFER 1024

/**
 * Socket 网络通信工具类
 * 提供TCP客户端/服务器通信的静态工具方法
 */
class Socket {
public:
    /**
     * 连接到TCP服务器
     * @param sock 输出参数，返回创建的套接字描述符
     * @param ip 服务器IP地址
     * @param port 服务器端口号
     * @throws 连接失败时通过标准错误输出错误信息并终止程序
     */
    static int connectToServer(int *sock, const  char* ip, int port);
    
    /**
     * 初始化TCP服务器
     * @param ser_sock 输出参数，返回监听套接字描述符
     * @param cli_sock 输出参数，返回客户端连接套接字描述符
     * @throws 初始化失败时通过标准错误输出错误信息并终止程序
     */
    static void init(int *ser_sock, int *cli_sock);
    
    /**
     * 处理客户端连接
     * @param client_socket 客户端套接字描述符
     * @note 此方法应在线程中调用以支持并发处理多个客户端
     */
    static void handleClient(int client_socket);
};


#endif
