/**
 * @file info_updater.h
 * @brief Information updater abstract base class interface, defining uniform information update specifications
 * @details This file declares the base class for all information updaters, using pure virtual functions to force subclasses to implement specific update logic,
 *          By separating the interface from the implementation, it is easy to expand different types of information update functions (such as debugging output, TFT display, network transmission, etc.).
 */

#ifndef INFO_UPDATER_H
#define INFO_UPDATER_H

/**
 * @class InfoUpdater
 * @brief Information updater abstract base class, defining a unified interface for information updates
 * @details 作For all concrete information updaters, provide a pure virtual `update` method,
 *          Subclasses must implement this method according to their own functions (such as refreshing the TFT screen, printing debugging information, sending network data, etc.).
 *          Use interface design patterns to ensure consistency in the behaviour of different update modules within the system.
 */
class InfoUpdater {
public:
    /**
     * @brief Purely virtual method, performs specific information update operations
     * @details Subclasses must implement this method to update information related to their own responsibilities,
     *          For example: Debug information is updated and output to the console, and the TFT updater refreshes the screen display,
     *          The network updater sends data to the server, etc.
     */
    virtual void update() = 0;

    /**
     * @brief Virtual destructor, ensuring that resources are released correctly when the subclass is destroyed
     * @details Define a virtual destructor for the base class to avoid memory leaks caused by the destructor not being called correctly when deleting subclass objects.
     */
    virtual ~InfoUpdater() {}
};

#endif // INFO_UPDATER_H
