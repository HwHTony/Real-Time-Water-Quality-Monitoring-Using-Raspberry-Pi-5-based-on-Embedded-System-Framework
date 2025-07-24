#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>

#include "sock.h"

/**
 * @brief Connect to the TCP server (with retry mechanism)
 * @param sock The socket descriptor returned after a successful connection is established.
 * @param ip Server IP address string (such as "192.168.1.2")
 * @param port Server port number
 * @return 0Indicates success，-1 indicates failure
 * @note Perform 3 internal retries. If the operation fails, output the error message in the log.
 */
int Socket::connectToServer(int *sock, const char* ip, int port) {
    if (sock == nullptr || ip == nullptr || port < 1 || port > 65535) {
        std::cerr << "Error: Invalid parameters - sock cannot be null, IP cannot be null, port must be within the range of 1 to 65535." << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr;
    const int max_retries = 3;       // Maximum retry count
    int retry_count = 0;

    // Repeated connection attempts in a loop
    while (retry_count < max_retries) {
        // 1. Create a TCP socket (IPv4 protocol, byte stream)
        *sock = socket(AF_INET, SOCK_STREAM, 0);
        if (*sock < 0) {
            std::cerr << "Error: Socket creation failed - " << strerror(errno) << std::endl;
            return -1;
        }

        // 2. Configure the server address structure
        memset(&server_addr, 0, sizeof(server_addr));  // Initialize the address structure
        server_addr.sin_family = AF_INET;              // IPv4 Agreement
        server_addr.sin_port = htons(port);             // Port number is converted to network byte order (big-endian)

        // Convert IP address (string to network byte order)
        if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
            close(*sock);  // Release the current invalid socket
            retry_count++;
            std::cerr << "The 1st" << retry_count << "This attempt failed: Invalid IP address（" << ip << "）" << std::endl;
            sleep(1);  // Wait for 1 second before retrying.
            continue;
        }

        // 3. Attempt to connect to the server
        if (connect(*sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            close(*sock);  // Release the current invalid socket
            retry_count++;
            std::cerr << "The 1st" << retry_count << "This attempt failed: Connecting to the server（" << ip << ":" << port << "）Failure：" << strerror(errno) << std::endl;
            sleep(1);  // Wait for 1 second before retrying.
        } else {
            // Connection successful
            std::cout << "Successfully connected to the server：" << ip << ":" << port << std::endl;
            return 0; // Successful return0
        }
    }

    // Failed even after exceeding the maximum retry limit
    std::cerr << "Error: Exceeded the maximum number of retry attempts（" << max_retries << "This attempt failed: Connecting to the server" << std::endl;
    return -1;
}


/**
 * @brief Initialize the TCP server (create a listening socket and bind to the port)
 * @param ser_sock The server listens to the socket descriptor.
 * @param cli_sock The client-side connection socket descriptor (reserved; it should be obtained during the accept operation)
 * @note This function only performs the server initialization. It needs to be called externally via 'accept' to obtain the client connection.
 */
void Socket::init(int *ser_sock, int *cli_sock) {
    if (ser_sock == nullptr || cli_sock == nullptr) {
        throw std::invalid_argument("Invalid parameters: ser_sock and cli_sock cannot be null.");
    }

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // 1. Create a TCP listening socket
    *ser_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*ser_sock < 0) {
        throw std::runtime_error("Listening socket creation failed：" + std::string(strerror(errno)));
    }

    // 2. Set socket options (allowing port reuse to avoid port occupation when the server restarts)
    int opt = 1;
    if (setsockopt(*ser_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(*ser_sock);
        throw std::runtime_error("Failed to set socket options：" + std::string(strerror(errno)));
    }

    // 3. Configure the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;                  // IPv4 Agreement
    server_addr.sin_addr.s_addr = INADDR_ANY;          // Monitor all available network interfaces（0.0.0.0）
    server_addr.sin_port = htons(PORT);                // Port number converted to network byte order

    // 4. Bind the socket to the specified address and port
    if (bind(*ser_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(*ser_sock);
        throw std::runtime_error("Binding port（" + std::to_string(PORT) + "）Failure：" + std::string(strerror(errno)));
    }

    // 5. Start listening (maximum waiting connection queue length is 5)
    if (listen(*ser_sock, 5) < 0) {
        close(*ser_sock);
        throw std::runtime_error("Listening port（" + std::to_string(PORT) + "）Failure：" + std::string(strerror(errno)));
    }

    // Initialize the client socket (default -1 indicates not connected)
    *cli_sock = -1;

    std::cout << "Server initialization is complete. Currently, it is at the port... " << PORT << " Listening to the connection..." << std::endl;
}


/**
 * @brief Handling client connections (receiving data and echoing)
 * @param client_socket Client socket descriptor
 * @note It is recommended to be called in an independent thread, which supports concurrent processing of multiple clients.
 */
void Socket::handleClient(int client_socket) {
    if (client_socket < 0) {
        throw std::invalid_argument("Invalid client socket：" + std::to_string(client_socket));
    }

    char buffer[BUFFER];  // Data buffer zone
    ssize_t bytes_read;   // Received byte count

    std::cout << "Start processing the client (socket)：" << client_socket << "）data..." << std::endl;

    // Repeatedly receive client data
    while ((bytes_read = recv(client_socket, buffer, BUFFER - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';  // Make sure the string ends with a null character.
        std::cout << "Received from the client（" << client_socket << "）data：" << buffer << std::endl;

        // Send the echoed data back to the client (which can be extended to business logic processing)
        if (send(client_socket, buffer, bytes_read, 0) < 0) {
            std::cerr << "To the client（" << client_socket << "）Data transmission failed：" << strerror(errno) << std::endl;
            break;
        }
    }

    // Handle connection termination or errors
    if (bytes_read == 0) {
        std::cout << "Client side（" << client_socket << "）Initiate the disconnection of the connection" << std::endl;
    } else if (bytes_read < 0) {
        std::cerr << "Receiving client（" << client_socket << "）There is an error in the data.：" << strerror(errno) << std::endl;
    }

    // Close the client socket and release the resources.
    close(client_socket);
    std::cout << "Client side（" << client_socket << "）Connection has been closed." << std::endl;
}
