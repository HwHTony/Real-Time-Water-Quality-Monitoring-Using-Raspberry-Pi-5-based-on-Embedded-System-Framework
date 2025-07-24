#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>  // Smart pointers manage resources
#include <QJsonObject>

// Forward declaration to reduce header file dependency
class QTcpServer;
class QTcpSocket;
class QPushButton;
namespace Ui { class MainWindow; }

/**
 * @brief Main window class, responsible for UI display and user interaction
 * Responsibilities: Only handle UI logic, not directly rely on network details, decoupled from the business layer through interfaces
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // Prohibit copying and moving to avoid resource management issues
    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;

public slots:
    /**
     * @brief Receive sensor data from the business layer and update the UI
     * @param data A JSON object containing "tur", "tmp", and "pH"
     */
    void onSensorDataUpdated(const QJsonObject& data);

    /**
     * @brief Update connection status display
     * @param status Status description string（such as:"Connected: 192.168.1.100"）
     */
    void onConnectionStatusChanged(const QString& status);

private:
    /**
     * @brief Initialize UI components (button style, layout, etc.)
     */
    void initUI();

    /**
     * @brief Set button style (encapsulate UI style logic)
     * @param button Target button
     * @param iconPath Icon resource path
     */
    void setButtonStyle(QPushButton* button, const QString& iconPath);

     void setButtonTransParent();  // Declare transparent background setting method

private:
    Ui::MainWindow* ui;  // UI form pointer
};

#endif // MAINWINDOW_H
