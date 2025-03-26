#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QHostAddress>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tcpServer(new QTcpServer(this))
    , clientSocket(nullptr)
{
    ui->setupUi(this);
    setupServer();
}

void MainWindow::setupServer()
{
    // 启动服务器并监听端口
    if (!tcpServer->listen(QHostAddress::Any, 12345)) {
        QMessageBox::critical(this, "Server Error",
                             tr("Unable to start server: %1")
                             .arg(tcpServer->errorString()));
        return;
    }

    // 连接新连接信号
    connect(tcpServer, &QTcpServer::newConnection,
            this, &MainWindow::handleNewConnection);

    ui->statusLabel->setText("🟢 Server Running (Port: 12345)");
}

void MainWindow::handleNewConnection()
{
    // 拒绝已有连接的新请求
    if (clientSocket) {
        QTcpSocket *newSocket = tcpServer->nextPendingConnection();
        newSocket->write("Another client is already connected!");
        newSocket->disconnectFromHost();
        return;
    }

    // 获取客户端套接字
    clientSocket = tcpServer->nextPendingConnection();

    connect(clientSocket, &QTcpSocket::disconnected,
            this, &MainWindow::clientDisconnected);

    ui->statusLabel->setText("🔵 Client Connected: " +
                            clientSocket->peerAddress().toString());
}

void MainWindow::clientDisconnected()
{
    clientSocket->deleteLater();
    clientSocket = nullptr;
    dataBuffer.clear();
    ui->statusLabel->setText("🟠 Client Disconnected");
}

MainWindow::~MainWindow()
{
    delete ui;
}
