#include <QApplication> 

#include "mainwindow.h" 


/**
 * @brief Program entry function
 * @param argc The number of command line arguments
 * @param argv Array of command line arguments (array of string pointers)
 * @return Status code when the program exits, 0 means normal exit
 */
int main(int argc, char *argv[])
{
    // Creating a Qt Application Instance
    // QApplication Manages the application life cycle, event loop, and resources, and is an essential object for Qt GUI programs
    QApplication qApplication(argc, argv);

    // Create the main window object
    // MainWindow It is a user-defined main window class, inherited from QMainWindow or QWidget
    MainWindow mainWindow;

    // Show Main Window
    // Call the show() method to make the window visible (the window is invisible when created by default)
    mainWindow.show();

    // Start the event loop of the Qt application
    // The event loop is responsible for processing user input (such as mouse and keyboard events), system events, etc. It is the core of Qt program operation.
    // The function blocks until the application exits (for example, the user closes all windows) and returns an exit status code.
    return qApplication.exec();
}
