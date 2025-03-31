#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

#include <QPushButton>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


public slots:
    void handleNewConnection();
    void clientDisconnected();

    void readClientData();
private:
    void setupServer();
    /* 设置按钮的样式 */
    void setButtonStyle(QPushButton * button, const QString & filename);
    /* 初始化按钮 */
    void initButtons();

    /* 设置按钮透明 */
    void setButtonTransParent();

    void processJsonBuffer();

    void updateSensorValues(const QJsonObject &json);
private:
    Ui::MainWindow *ui;

    QTcpServer *tcpServer;
    QTcpSocket *clientSocket;
    QByteArray dataBuffer; // For storing incomplete data
};

#endif // MAINWINDOW_H
