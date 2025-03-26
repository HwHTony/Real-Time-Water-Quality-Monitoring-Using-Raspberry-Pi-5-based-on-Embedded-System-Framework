#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>



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

private:
    void setupServer();

private:
    Ui::MainWindow *ui;

    QTcpServer *tcpServer;
    QTcpSocket *clientSocket;
    QByteArray dataBuffer; // 用于累积接收的数据
};

#endif // MAINWINDOW_H
