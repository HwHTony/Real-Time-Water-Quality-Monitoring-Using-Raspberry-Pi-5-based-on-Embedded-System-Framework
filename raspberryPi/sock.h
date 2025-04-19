#ifndef SOCK_H
#define SOCK_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "com.h"

#define PORT 8888
#define BUFFER 1024
// void serv_connect(int *sock, char* ip, int port);

class Socket {
    public:
        static void connectToServer(int *sock, char* ip, int port);
        static void init(int *ser_sock, int *cli_sock);
        static void handleClient(int client_socket);
    };


#endif
