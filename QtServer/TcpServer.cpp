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
    // Start server listening
    if (!tcpServer->listen(QHostAddress::Any, port)) {
        emit connectionStatusChanged("🔴 Server Error: " + tcpServer->errorString());
        return;
    }
    
    // Connect new client signal
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

// Handling new client connections
void TcpServer::onNewConnection()
{
    // Only one client connection is allowed
    if (clientSocket) {
        QTcpSocket* newSocket = tcpServer->nextPendingConnection();
        newSocket->write("Another client is already connected!");
        newSocket->disconnectFromHost();
        newSocket->deleteLater();
        return;
    }
    
    // Save the client socket and connect the signal
    clientSocket = tcpServer->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::disconnected, this, &TcpServer::onClientDisconnected);
    connect(clientSocket, &QTcpSocket::readyRead, this, &TcpServer::onReadyRead);
    
    emit connectionStatusChanged("🔵 Client Connected: " + clientSocket->peerAddress().toString());
}

// Handling client disconnects
void TcpServer::onClientDisconnected()
{
    emit connectionStatusChanged("🟠 Client Disconnected");
    clientSocket->deleteLater();
    clientSocket = nullptr;
    dataBuffer.clear();
}

// Read client data
void TcpServer::onReadyRead()
{
    if (!clientSocket) return;
    
    // Read all data into the buffer
    QByteArray data = clientSocket->readAll();
    dataBuffer.append(data);
    qDebug() << "Received data: " << data;
    
    // Parse JSON data in the buffer
    parseJsonData(dataBuffer);
}

// Parsing JSON data and emitting signals
void TcpServer::parseJsonData(QByteArray &buffer)
{
    // Find the complete JSON object（{...}）
    int start = buffer.indexOf('{');
    int end = buffer.indexOf('}', start);
    
    while (start != -1 && end != -1) {
        // Extract and remove parsed JSON fragments
        QByteArray jsonData = buffer.mid(start, end - start + 1);
        buffer.remove(0, end + 1);
        
        // Parsing JSON
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);
        if (error.error == QJsonParseError::NoError) {
            emit sensorDataUpdated(doc.object()); // Emit parsed data
        } else {
            qDebug() << "JSON parse error: " << error.errorString();
            emit connectionStatusChanged("🟠 JSON Error: " + error.errorString());
        }
        
        // Continue to search for the next JSON object
        start = buffer.indexOf('{');
        end = buffer.indexOf('}', start);
    }
}
