// TcpServer.h
#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>

/**
 * @brief TCP服务器类，负责网络通信逻辑
 * 职责：处理客户端连接、数据收发、JSON解析，与UI层通过信号解耦
 */
class TcpServer : public QObject
{
    Q_OBJECT

public:
    explicit TcpServer(quint16 port = 8888, QObject* parent = nullptr);
    ~TcpServer() override;

signals:
    /**
     * @brief 当接收到有效传感器数据时发射
     * @param data 解析后的JSON对象
     */
    void sensorDataUpdated(const QJsonObject& data);

    /**
     * @brief 当连接状态变化时发射
     * @param status 状态描述
     */
    void connectionStatusChanged(const QString& status);

private slots:
    void onNewConnection();       // 处理新连接
    void onClientDisconnected();  // 处理客户端断开
    void onReadyRead();           // 读取客户端数据

private:
    /**
     * @brief 解析缓冲区中的JSON数据
     * @param buffer 待解析的字节缓冲区
     */
    void parseJsonData(QByteArray& buffer);

private:
    QTcpServer* tcpServer;        // TCP服务器实例
    QTcpSocket* clientSocket;     // 当前连接的客户端套接字
    QByteArray dataBuffer;        // 数据缓冲区（处理粘包/拆包）
    quint16 port;                 // 监听端口
};

#endif // TCPSERVER_H
