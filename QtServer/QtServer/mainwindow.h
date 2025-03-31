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
    /* set buttons styles. */
    void setButtonStyle(QPushButton * button, const QString & filename);
    /* init buttons. */
    void initButtons();
    /* set button transparent. */
    void setButtonTransParent();
    /* parse json data. */
    void processJsonBuffer();
    /* update senor value. */
    void updateSensorValues(const QJsonObject &json);
private:
    Ui::MainWindow *ui;

    QTcpServer *tcpServer;
    QTcpSocket *clientSocket;
    QByteArray dataBuffer; // For storing incomplete data
};

#endif // MAINWINDOW_H
