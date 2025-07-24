#ifndef SOCK_H
#define SOCK_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../common/com.h"

#define PORT 8888
#define BUFFER 1024

/**
 * Socket Network communication tools category
 * Provide static tool methods for TCP client/server communication
 */
class Socket {
public:
    /**
     * Connect to the TCP server
     * @param sock Output parameter: Returns the created socket descriptor
     * @param ip Server IP address
     * @param port Server port number
     * @throws When the connection fails, an error message is output through standard error and the program is terminated.
     */
    static int connectToServer(int *sock, const  char* ip, int port);
    
    /**
     * Initialize the TCP server
     * @param ser_sock Output parameter: Returns the descriptor of the listening socket
     * @param cli_sock Output parameter: Returns the client connection socket descriptor
     * @throws When initialization fails, error information is output through standard error and the program is terminated.
     */
    static void init(int *ser_sock, int *cli_sock);
    
    /**
     * Handle client connections
     * @param client_socket Client socket descriptor
     * @note This method should be called within the thread to support concurrent processing of multiple clients.
     */
    static void handleClient(int client_socket);
};


#endif
