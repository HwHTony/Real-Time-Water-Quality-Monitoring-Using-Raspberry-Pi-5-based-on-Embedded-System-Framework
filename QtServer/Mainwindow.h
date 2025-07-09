#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>  // 智能指针管理资源
#include <QJsonObject>

// 前向声明，减少头文件依赖
class QTcpServer;
class QTcpSocket;
class QPushButton;
namespace Ui { class MainWindow; }

/**
 * @brief 主窗口类，负责UI展示和用户交互
 * 职责：仅处理UI逻辑，不直接依赖网络细节，通过接口与业务层解耦
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // 禁止拷贝和移动，避免资源管理问题
    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;

public slots:
    /**
     * @brief 从业务层接收传感器数据并更新UI
     * @param data 包含"tur"、"tmp"、"pH"的JSON对象
     */
    void onSensorDataUpdated(const QJsonObject& data);

    /**
     * @brief 更新连接状态显示
     * @param status 状态描述字符串（如"Connected: 192.168.1.100"）
     */
    void onConnectionStatusChanged(const QString& status);

private:
    /**
     * @brief 初始化UI组件（按钮样式、布局等）
     */
    void initUI();

    /**
     * @brief 设置按钮样式（封装UI样式逻辑）
     * @param button 目标按钮
     * @param iconPath 图标资源路径
     */
    void setButtonStyle(QPushButton* button, const QString& iconPath);

     void setButtonTransParent();  // 声明透明背景设置方法

private:
    Ui::MainWindow* ui;  // UI表单指针
};

#endif // MAINWINDOW_H
