// TcpServer.h
#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>

/**
 * @brief TCP server class, responsible for network communication logic
 * Responsibilities: handle client connections, data sending and receiving, JSON parsing, and decouple from the UI layer through signals
 */
class TcpServer : public QObject
{
    Q_OBJECT

public:
    explicit TcpServer(quint16 port = 8888, QObject* parent = nullptr);
    ~TcpServer() override;

signals:
    /**
     * @brief Emitted when valid sensor data is received
     * @param data Parsed JSON object
     */
    void sensorDataUpdated(const QJsonObject& data);

    /**
     * @brief Emitted when the connection state changes
     * @param status Status description
     */
    void connectionStatusChanged(const QString& status);

private slots:
    void onNewConnection();       // Handling new connections
    void onClientDisconnected();  // Handling Client Disconnects
    void onReadyRead();           // Read client data

private:
    /**
     * @brief Parse JSON data in the buffer
     * @param buffer The byte buffer to be parsed
     */
    void parseJsonData(QByteArray& buffer);

private:
    QTcpServer* tcpServer;        // TCP Server Example
    QTcpSocket* clientSocket;     // The currently connected client socket
    QByteArray dataBuffer;        // Data buffer (handling packet sticking/unpacking)
    quint16 port;                 // Listening Port
};

#endif // TCPSERVER_H
