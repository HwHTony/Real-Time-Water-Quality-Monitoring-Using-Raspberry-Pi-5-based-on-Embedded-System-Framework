#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tcpServer(new QTcpServer(this))
    , clientSocket(nullptr)
{
    ui->setupUi(this);

    setWindowTitle("Water quality");

    /*init buttons. */
    initButtons();


    ui->phVal->setText("0.0");
    ui->tempVal->setText("0.0℃");
    ui->turbidVal->setText("0.0");
    setupServer();
}


/* 设置按钮透明 */
void MainWindow::setButtonTransParent()
{
    /* set buttons backgrounds */
    ui->phVal->setStyleSheet("background-color:transparent");
    /* set buttons backgrounds */
    ui->tempVal->setStyleSheet("background-color:transparent");
    /* set buttons backgrounds */
    ui->turbidVal->setStyleSheet("background-color:transparent");
}

/* 设置按钮的样式 */
void MainWindow::setButtonStyle(QPushButton * button, const QString & filename)
{
    /* set icons */
    button->setIcon(QIcon(filename));
    /* set icons size */
    button->setIconSize(QSize(button->width(), button->height()));
    /* set button background transparent. */
    button->setStyleSheet("background-color:transparent");
}

/* 初始化按钮 */
void MainWindow::initButtons()
{
    setButtonStyle(ui->phBtn, ":/icons/ph.png");
    setButtonStyle(ui->tempBtn, ":/icons/temp.png");
    setButtonStyle(ui->turbidBtn, ":/icons/turbid.png");

    /* set appoint buttons transparent. */
    setButtonTransParent();
}
void MainWindow::setupServer()
{
    // 启动服务器并监听端口
    if (!tcpServer->listen(QHostAddress::Any, 8888))
    {
        QMessageBox::critical(this, "Server Error",
                             tr("Unable to start server: %1")
                             .arg(tcpServer->errorString()));
        return;
    }
    // new connection.
    connect(tcpServer, &QTcpServer::newConnection, this, &MainWindow::handleNewConnection);
}

void MainWindow::handleNewConnection()
{
    // refuse connected socket
    if (clientSocket)
    {
        QTcpSocket *newSocket = tcpServer->nextPendingConnection();
        newSocket->write("Another client is already connected!");
        newSocket->disconnectFromHost();
        return;
    }

    // get client socket
    clientSocket = tcpServer->nextPendingConnection();

    connect(clientSocket, &QTcpSocket::disconnected, this, &MainWindow::clientDisconnected);
    connect(clientSocket, &QTcpSocket::readyRead, this, &MainWindow::readClientData);

    ui->statusLabel->setText("🔵 Client Connected: " + clientSocket->peerAddress().toString());
}

void MainWindow::readClientData()
{
    if (!clientSocket)
        return;

    // Read all available data
    QByteArray data = clientSocket->readAll();
    dataBuffer.append(data);

    qDebug() << "data:" << data << endl;

    // Try to process complete JSON messages
    processJsonBuffer();
}

void MainWindow::processJsonBuffer()
{
    // Find the start and end of a complete JSON object
    int start = dataBuffer.indexOf('{');
    int end = dataBuffer.indexOf('}', start);

    while (start != -1 && end != -1) {
        // Extract the JSON object
        QByteArray jsonData = dataBuffer.mid(start, end - start + 1);
        dataBuffer.remove(0, end + 1); // Remove processed data

        // Parse JSON
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);

        if (error.error == QJsonParseError::NoError) {
            updateSensorValues(doc.object());
        } else {
            qDebug() << "JSON parse error:" << error.errorString();
        }

        // Look for next JSON object
        start = dataBuffer.indexOf('{');
        end = dataBuffer.indexOf('}', start);
    }
}

void MainWindow::updateSensorValues(const QJsonObject &json)
{
    // Update UI with new values
    if (json.contains("tur")) {
        ui->turbidVal->setText(QString::number(json["tur"].toString().toDouble(), 'f', 2));
    }
    if (json.contains("tmp")) {
        ui->tempVal->setText(json["tmp"].toString() + "℃");
    }
    if (json.contains("pH")) {
        ui->phVal->setText(json["pH"].toString());
    }
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
