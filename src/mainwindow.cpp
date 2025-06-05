#include "mainwindow.h"
#include "canvaswidget.h"   // 核心画布组件
#include "treenode.h"
#include <QMenuBar>         // 菜单栏
#include <QAction>          // 菜单动作
#include <QFileDialog>      // 文件对话框
#include <QApplication>     // 应用全局对象
#include <qstandardpaths.h>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 初始化窗口属性
    setWindowTitle("矩形树图绘制器");
    resize(800, 600);  // 默认窗口尺寸

    // 创建核心画布组件并设为中央部件
    m_canvasWidget = new CanvasWidget(this);
    setCentralWidget(m_canvasWidget);

    // 初始化菜单系统
    createMenu();
}

MainWindow::~MainWindow()
{
    // 所有Qt对象通过父子关系自动释放，无需手动删除
}

void MainWindow::createMenu()
{
    // 创建"文件"菜单
    QMenu *fileMenu = menuBar()->addMenu(tr("文件"));

    // 新建动作
    m_newAction = new QAction(tr("新建(&N)"), this);
    m_newAction->setShortcut(QKeySequence::New);  // 绑定Ctrl+N
    connect(m_newAction, &QAction::triggered, this, &MainWindow::onNew);
    fileMenu->addAction(m_newAction);

    // 保存动作
    m_saveAction = new QAction(tr("保存(&S)"), this);
    m_saveAction->setShortcut(QKeySequence::Save); // 绑定Ctrl+S
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::onSave);
    fileMenu->addAction(m_saveAction);

    // 保存为pdf动作
    m_pdfAction = new QAction(tr("保存为pdf(&P)"),this);
    m_pdfAction->setShortcut(QKeySequence("Ctrl+P"));
    connect(m_pdfAction,&QAction::triggered,this,&MainWindow::onPdf);
    fileMenu->addAction(m_pdfAction);

    // 打开动作
    m_openAction = new QAction(tr("打开(&O)"), this);
    m_openAction->setShortcut(QKeySequence::Open); // 绑定Ctrl+O
    connect(m_openAction, &QAction::triggered, this, &MainWindow::onOpen);
    fileMenu->addAction(m_openAction);

    // 创建矩形菜单
    QMenu *recMenu = menuBar()->addMenu(tr("矩形"));

    // 新建矩形动作
    m_recAction = new QAction(tr("新建矩形(&R)"), this);
    m_recAction->setShortcut(QKeySequence("Ctrl+R"));  // 绑定Ctrl+R
    connect(m_recAction, &QAction::triggered, this, &MainWindow::onRec);
    recMenu->addAction(m_recAction);
}

void MainWindow::onNew()
{
    // 清空画布现有内容
    m_canvasWidget->clear();
}

void MainWindow::onSave()
{
    // 获取保存路径
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("保存文件"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        tr("文本文件 (*.txt)")
        );

    if (!filePath.isEmpty()) {
        // 确保文件后缀正确
        if (!filePath.endsWith(".txt", Qt::CaseInsensitive)) {
            filePath += ".txt";
        }
        // 委托画布执行保存操作
        m_canvasWidget->saveToFile(filePath);
    }
}
void MainWindow::onPdf(){
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("保存文件"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        tr("文本文件 (*.pdf)")
        );

    if (!filePath.isEmpty()) {
        // 确保文件后缀正确
        if (!filePath.endsWith(".pdf", Qt::CaseInsensitive)) {
            filePath += ".pdf";
        }
        // 委托画布执行保存操作
        m_canvasWidget->savetopdf(filePath);
    }
}
void MainWindow::onRec(){
    // 在画布中央创建初始矩形
    const QRect canvasRect = m_canvasWidget->rect();
    const QPoint center = canvasRect.center();

    // 初始矩形尺寸（可自定义）
    const int initWidth = 120;
    const int initHeight = 80;
    QRect initRect(
        center.x() - initWidth/2,
        center.y() - initHeight/2,
        initWidth,
        initHeight
        );

    // 通知画布添加新节点
    m_canvasWidget->addTreeNode(initRect, "新建节点");
}

// == 文件打开 ==
void MainWindow::onOpen()
{
    // 获取打开路径
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("打开文件"), "",
                                                tr("文本文件 (*.txt);;所有文件 (*)"));

    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("错误"), tr("无法打开文件"));
        return;
    }

    // 清空当前场景
    m_canvasWidget->clear();

    QTextStream in(&file);
    QVector<TreeNode*> nodes; // 存储加载的节点
    QString currentSection;
    int lineCount = 0;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineCount++;

        if (line.isEmpty()) continue;

        // 检测段标题
        if (line.startsWith('[') && line.endsWith(']')) {
            currentSection = line.mid(1, line.length() - 2);
            continue;
        }

        if (currentSection == "Nodes") {
            // 解析节点行: ID,x,y,width,height,text
            QStringList parts = line.split(',');
            if (parts.size() < 6) {
                qWarning() << "无效节点行 #" << lineCount << ":" << line;
                continue;
            }

            bool ok;
            int id = parts[0].toInt(&ok);
            if (!ok || id < 0) continue;

            int x = parts[1].toInt(&ok);
            if (!ok) continue;

            int y = parts[2].toInt(&ok);
            if (!ok) continue;

            int width = parts[3].toInt(&ok);
            if (!ok) continue;

            int height = parts[4].toInt(&ok);
            if (!ok) continue;

            // 合并文本部分（可能包含逗号）
            QString text = parts.mid(5).join(",");

            // 创建节点
            const QRect canvasRect = m_canvasWidget->rect();
            const QPoint center = canvasRect.center();

            // 初始矩形尺寸（可自定义）
            QRect initRect(
                x,
                y,
                width,
                height
                );

            // 通知画布添加新节点
            TreeNode* newnode = m_canvasWidget->addTreeNode(initRect, text);
            if (id >= nodes.size()) {
                nodes.resize(id + 1);
            }
            nodes[id] = newnode;
        }
        else if (currentSection == "Connections") {
            // 解析连接行: startID,endID
            QStringList parts = line.split(',');
            if (parts.size() != 2) {
                qWarning() << "无效连接行 #" << lineCount << ":" << line;
                continue;
            }

            bool ok;
            int startID = parts[0].toInt(&ok);
            if (!ok || startID < 0) continue;

            int endID = parts[1].toInt(&ok);
            if (!ok || endID < 0) continue;

            // 检查节点ID是否有效
            if (startID >= nodes.size() || endID >= nodes.size() ||
                !nodes[startID] || !nodes[endID]) {
                qWarning() << "无效节点ID在连接行 #" << lineCount;
                continue;
            }

            // 创建连接
            m_canvasWidget->addConnection(nodes[startID],nodes[endID]);
        }
    }

    file.close();

    // 刷新画布
    m_canvasWidget->update();
}
