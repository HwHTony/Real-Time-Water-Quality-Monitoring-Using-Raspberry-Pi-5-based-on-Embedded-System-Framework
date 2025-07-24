/**
 * @file socket_info_updater.h
 * @brief Network socket information updater class interface, responsible for sending water quality data over the network
 * @details Defines the functionality to send real-time water quality monitoring data to the server through the connected socket,
 *          Inherited from the InfoUpdater abstract base class, it implements the specific logic for sending network data,
 *          It is the core module of ‘data output to the network’ in the system.
 */

#ifndef SOCKET_INFO_UPDATER_H
#define SOCKET_INFO_UPDATER_H

#include "../common/com.h"              // Public types and utility function definitions
#include "../common/water_quality.h"    // Water quality data single instance class, used to obtain data to be sent
#include "info_updater.h"               // Information updater base class, providing a unified update interface

/**
 * @class SocketInfoUpdater
 * @brief A socket-based information updater responsible for sending real-time water quality data to the server via the network
 * @details Inherited from the InfoUpdater abstract base class, it receives a connected socket descriptor through the constructor,
 *          Implement data retrieval, formatting, and transmission in the rewritten update method, typically in conjunction with a timer
 *          Periodically (e.g., once per second) push the latest data to the server.
 */
class SocketInfoUpdater: public InfoUpdater {
private:
    int sock;  ///< A socket descriptor that has been established for communication with the server, passed in by the constructor

public:
    /**
     * @brief Constructor, initialise socket members
     * @param s Connected socket descriptor (must be created in advance using Socket::connectToServer)
     * @note The socket must be in a connected state, otherwise subsequent update methods may fail to send
     */
    SocketInfoUpdater(int s);

    /**
     * @brief Override the pure virtual method of the base class to execute network data transmission
     * @details Retrieve the latest water quality data (temperature, pH value, turbidity, etc.) from the WaterQuality singleton,
     *          After formatting according to the preset format (such as string, JSON, etc.), it is sent to the server via a socket,
     *          If the transmission fails, an error message will be output (the specific error handling logic is determined by the implementation).
     */
    void update() override;
};

#endif // SOCKET_INFO_UPDATER_H
