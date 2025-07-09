#include <QMessageBox>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpserver.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Water Quality Monitor");
    
    // 初始化UI组件
    initUI();
    
    // 初始化TCP服务器（业务层）并建立信号槽连接
    TcpServer* tcpServer = new TcpServer(8888, this);
    connect(tcpServer, &TcpServer::sensorDataUpdated,
            this, &MainWindow::onSensorDataUpdated);
    connect(tcpServer, &TcpServer::connectionStatusChanged,
            this, &MainWindow::onConnectionStatusChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 初始化UI（按钮样式、初始值等）
void MainWindow::initUI()
{
    // 初始化传感器显示值
    ui->phVal->setText("0.0");
    ui->tempVal->setText("0.0℃");
    ui->turbidVal->setText("0.0");
    
    // 设置按钮样式
    setButtonStyle(ui->phBtn, ":/icons/ph.png");
    setButtonStyle(ui->tempBtn, ":/icons/temp.png");
    setButtonStyle(ui->turbidBtn, ":/icons/turbid.png");
    
    // 设置文本框透明背景
    setButtonTransParent();
}

// 设置按钮样式（封装UI细节）
void MainWindow::setButtonStyle(QPushButton *button, const QString &iconPath)
{
    button->setIcon(QIcon(iconPath));
    button->setIconSize(QSize(button->width(), button->height()));
    button->setStyleSheet("background-color:transparent");
}

// 设置文本框透明背景
void MainWindow::setButtonTransParent()
{
    ui->phVal->setStyleSheet("background-color:transparent");
    ui->tempVal->setStyleSheet("background-color:transparent");
    ui->turbidVal->setStyleSheet("background-color:transparent");
}

// 接收业务层数据并更新UI
void MainWindow::onSensorDataUpdated(const QJsonObject &data)
{
    if (data.contains("tur")) {
        double tur = data["tur"].toDouble();
        ui->turbidVal->setText(QString::number(tur, 'f', 2));
    }
    if (data.contains("tmp")) {
        double tmp = data["tmp"].toDouble();
        ui->tempVal->setText(QString::number(tmp, 'f', 2) + "℃");
    }
    if (data.contains("pH")) {
        double ph = data["pH"].toDouble();
        ui->phVal->setText(QString::number(ph, 'f', 2));
    }
}

// 接收连接状态并更新UI
void MainWindow::onConnectionStatusChanged(const QString &status)
{
    ui->statusLabel->setText(status);
}