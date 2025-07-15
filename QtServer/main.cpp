#include <QApplication> 

#include "mainwindow.h" 


/**
 * @brief 程序入口函数
 * @param argc 命令行参数的数量
 * @param argv 命令行参数的数组（字符串指针数组）
 * @return 程序退出时的状态码，0表示正常退出
 */
int main(int argc, char *argv[])
{
    // 创建Qt应用程序实例
    // QApplication管理应用程序的生命周期、事件循环和资源，是Qt GUI程序的必备对象
    QApplication qApplication(argc, argv);

    // 创建主窗口对象
    // MainWindow是用户自定义的主窗口类，继承自QMainWindow或QWidget
    MainWindow mainWindow;

    // 显示主窗口
    // 调用show()方法使窗口可见（默认创建时窗口不可见）
    mainWindow.show();

    // 启动Qt应用程序的事件循环
    // 事件循环负责处理用户输入（如鼠标、键盘事件）、系统事件等，是Qt程序运行的核心
    // 函数会阻塞直到应用程序退出（如用户关闭所有窗口），返回退出状态码
    return qApplication.exec();
}