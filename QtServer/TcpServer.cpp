#include "tcpserver.h"
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>
#include <QMessageBox>

TcpServer::TcpServer(quint16 port, QObject *parent)
    : QObject(parent)
    , port(port)
    , tcpServer(new QTcpServer(this))
    , clientSocket(nullptr)
{
    // 启动服务器监听
    if (!tcpServer->listen(QHostAddress::Any, port)) {
        emit connectionStatusChanged("🔴 Server Error: " + tcpServer->errorString());
        return;
    }
    
    // 连接新客户端信号
    connect(tcpServer, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
    emit connectionStatusChanged("🟢 Server started on port " + QString::number(port));
}

TcpServer::~TcpServer()
{
    if (clientSocket) {
        clientSocket->disconnectFromHost();
        clientSocket->deleteLater();
    }
}

// 处理新客户端连接
void TcpServer::onNewConnection()
{
    // 仅允许一个客户端连接
    if (clientSocket) {
        QTcpSocket* newSocket = tcpServer->nextPendingConnection();
        newSocket->write("Another client is already connected!");
        newSocket->disconnectFromHost();
        newSocket->deleteLater();
        return;
    }
    
    // 保存客户端套接字并连接信号
    clientSocket = tcpServer->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::disconnected, this, &TcpServer::onClientDisconnected);
    connect(clientSocket, &QTcpSocket::readyRead, this, &TcpServer::onReadyRead);
    
    emit connectionStatusChanged("🔵 Client Connected: " + clientSocket->peerAddress().toString());
}

// 处理客户端断开连接
void TcpServer::onClientDisconnected()
{
    emit connectionStatusChanged("🟠 Client Disconnected");
    clientSocket->deleteLater();
    clientSocket = nullptr;
    dataBuffer.clear();
}

// 读取客户端数据
void TcpServer::onReadyRead()
{
    if (!clientSocket) return;
    
    // 读取所有数据到缓冲区
    QByteArray data = clientSocket->readAll();
    dataBuffer.append(data);
    qDebug() << "Received data: " << data;
    
    // 解析缓冲区中的JSON数据
    parseJsonData(dataBuffer);
}

// 解析JSON数据并发射信号
void TcpServer::parseJsonData(QByteArray &buffer)
{
    // 查找完整的JSON对象（{...}）
    int start = buffer.indexOf('{');
    int end = buffer.indexOf('}', start);
    
    while (start != -1 && end != -1) {
        // 提取并移除已解析的JSON片段
        QByteArray jsonData = buffer.mid(start, end - start + 1);
        buffer.remove(0, end + 1);
        
        // 解析JSON
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);
        if (error.error == QJsonParseError::NoError) {
            emit sensorDataUpdated(doc.object()); // 发射解析后的数据
        } else {
            qDebug() << "JSON parse error: " << error.errorString();
            emit connectionStatusChanged("🟠 JSON Error: " + error.errorString());
        }
        
        // 继续查找下一个JSON对象
        start = buffer.indexOf('{');
        end = buffer.indexOf('}', start);
    }
}