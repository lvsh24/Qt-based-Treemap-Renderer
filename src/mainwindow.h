#pragma once

#include <QMainWindow>

// 前向声明（避免头文件相互包含）
class CanvasWidget;

/**
 * @brief 主窗口类，负责管理应用程序的主界面框架
 * 包含菜单栏和核心画布组件，处理文件操作相关信号
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT  // Qt元对象系统宏

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针（默认为nullptr）
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~MainWindow() override;

private slots:
    /**
     * @brief 处理"新建"菜单动作的槽函数
     * 1. 清除当前所有图形元素
     * 2. 在画布中央创建初始矩形块
     */
    void onNew();

    /**
     * @brief 处理"保存"菜单动作的槽函数
     * 将当前画布内容保存为文本文件，使用QFileDialog选择路径
     * 文件格式参见项目框架设计文档
     */
    void onSave();

    void onRec();

    void onOpen();

    void onPdf();

private:
    // 核心画布组件（负责所有图形元素的绘制和交互）
    CanvasWidget *m_canvasWidget;

    // 菜单栏动作
    QAction *m_newAction;   // "新建"动作
    QAction *m_saveAction;  // "保存"动作
    QAction *m_recAction;
    QAction *m_openAction;
    QAction *m_pdfAction;

    /**
     * @brief 初始化菜单栏
     *
     * 创建"文件"菜单并添加动作：
     * - 新建 (Ctrl+N)
     * - 保存 (Ctrl+S)
     */
    void createMenu();
};
