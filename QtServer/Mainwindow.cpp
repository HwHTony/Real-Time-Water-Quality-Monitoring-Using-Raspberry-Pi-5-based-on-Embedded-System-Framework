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
    
    // Initializing UI components
    initUI();
    
    // Initialize the TCP server (business layer) and establish a signal-slot connection
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

// Initialize UI (button style, initial value, etc.)
void MainWindow::initUI()
{
    // Initialize sensor display value
    ui->phVal->setText("0.0");
    ui->tempVal->setText("0.0℃");
    ui->turbidVal->setText("0.0");
    
    // Setting the button style
    setButtonStyle(ui->phBtn, ":/icons/ph.png");
    setButtonStyle(ui->tempBtn, ":/icons/temp.png");
    setButtonStyle(ui->turbidBtn, ":/icons/turbid.png");
    
    // Set the text box transparent background
    setButtonTransParent();
}

// Set button style (encapsulate UI details)
void MainWindow::setButtonStyle(QPushButton *button, const QString &iconPath)
{
    button->setIcon(QIcon(iconPath));
    button->setIconSize(QSize(button->width(), button->height()));
    button->setStyleSheet("background-color:transparent");
}

// Set the text box transparent background
void MainWindow::setButtonTransParent()
{
    ui->phVal->setStyleSheet("background-color:transparent");
    ui->tempVal->setStyleSheet("background-color:transparent");
    ui->turbidVal->setStyleSheet("background-color:transparent");
}

// Receive business layer data and update the UI
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

// Receive connection status and update UI
void MainWindow::onConnectionStatusChanged(const QString &status)
{
    ui->statusLabel->setText(status);
}
