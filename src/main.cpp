#include "mainwindow.h"  // 主窗口头文件
#include <QApplication>   // Qt 应用框架核心头文件

/**
 * @brief 应用程序入口点
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 * @return 应用程序退出码
 */
int main(int argc, char *argv[])
{
    // === 初始化阶段 ===
    QApplication app(argc, argv);  // 创建 Qt 应用对象

// 启用高 DPI 缩放（Qt 5.6+ 特性）
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    // === 主窗口创建 ===
    MainWindow mainWindow;  // 实例化主窗口
    mainWindow.setWindowTitle("矩形树图绘制器 v1.0"); // 设置窗口标题
    mainWindow.show();       // 显示主窗口

    // === 事件循环 ===
    return app.exec();  // 进入 Qt 事件循环
}
