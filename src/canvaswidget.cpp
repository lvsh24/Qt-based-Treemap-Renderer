#include "canvaswidget.h"
#include "treenode.h"
#include "connection.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <Qfile>
#include <QtPrintSupport/QPrinter>
#include <QPdfWriter>
#include <QPageSize>

CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true); // 启用鼠标跟踪
    setFocusPolicy(Qt::StrongFocus); // 允许接收键盘事件
}

CanvasWidget::~CanvasWidget()
{
    // 释放所有节点和连接线内存
    qDeleteAll(m_nodes);
    qDeleteAll(m_connections);
}

// 清空所有元素
void CanvasWidget::clear()
{
    qDeleteAll(m_nodes);
    qDeleteAll(m_connections);
    m_nodes.clear();
    m_connections.clear();
    update();
}

// 添加新节点
TreeNode* CanvasWidget::addTreeNode(const QRect &rect, const QString &text) {
    // 创建节点时传入 this 指针（即所属 CanvasWidget）
    TreeNode* node = new TreeNode(this, rect, text);
    m_nodes.append(node);
    update();
    return node;
}

void CanvasWidget::addConnection(TreeNode *start,TreeNode *end){
    m_connections.append(new Connection(start,end));
    update();
}

// === 事件处理 ===
void CanvasWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制所有连接线
    for (Connection* conn : qAsConst(m_connections)) {
        conn->draw(&painter);
    }

    // 绘制所有节点
    for (TreeNode* node : qAsConst(m_nodes)) {
        node->draw(&painter, CONTROL_POINT_SIZE, PLUS_ICON_SIZE);

        // 高亮当前操作节点
        if (node == m_draggingNode || node == m_resizeNode) {
            painter.setPen(Qt::blue);
            painter.drawRect(node->geometry().adjusted(-1, -1, 1, 1));
        }
    }

    // 绘制临时连接线
    if (m_currentAction == CreatingConnection) {
        painter.setPen(Qt::black);
        painter.drawLine(m_connectionStartNode->center(), m_tempConnectionEnd);
    }
}

void CanvasWidget::mousePressEvent(QMouseEvent* event)
{
    QPoint pos = event->pos();
    TreeNode* node = findNodeAt(pos);

    if (event->button() == Qt::LeftButton) {
        if (node) {
            // 检查是否点击调整控制点
            QRect resizeArea(node->geometry().bottomRight() - QPoint(CONTROL_POINT_SIZE, CONTROL_POINT_SIZE),
                             QSize(CONTROL_POINT_SIZE * 2, CONTROL_POINT_SIZE * 2));
            if (resizeArea.contains(pos)) {
                m_currentAction = Resizing;
                m_resizeNode = node;
                m_dragStartPos = pos;
                return;
            }

            // 检查是否点击加号图标
            QRect plusArea(node->geometry().right() - PLUS_ICON_SIZE,
                           node->geometry().center().y() - PLUS_ICON_SIZE/2,
                           PLUS_ICON_SIZE, PLUS_ICON_SIZE);
            if (plusArea.contains(pos)) {
                m_currentAction = CreatingConnection;
                m_connectionStartNode = node;
                m_tempConnectionEnd = pos;
                return;
            }

            //检查是否点击文本框
            int delta1 = node->m_rect.height()/4;
            int delta2 = node->m_rect.width()/4;
            QRect textArea = node->m_rect.adjusted(delta2,delta1,-delta2,-delta1);
            if (textArea.contains(pos)) {
                m_currentAction = EditingText;
                m_editingNode = node;
                return;
            }

            // 普通拖拽
            m_currentAction = DraggingNode;
            m_draggingNode = node;
            m_nodeDragStartPos = node->geometry().topLeft();
            m_dragStartPos = pos;
        }
    }
}

void CanvasWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pos = event->pos();

    switch (m_currentAction) {
    case Resizing:
        handleResize(m_resizeNode, pos);
        break;

    case DraggingNode:
        m_draggingNode->moveTo(m_nodeDragStartPos + (pos - m_dragStartPos));
        updateConnectionPositions();
        update();
        break;

    case CreatingConnection:
        m_tempConnectionEnd = pos;
        update();
        break;

    default:
        // 悬停效果处理
        TreeNode* hoverNode = findNodeAt(pos);
        for (TreeNode* node : qAsConst(m_nodes)) {
            node->setHovered(node == hoverNode);
        }
        update();
    }
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        switch (m_currentAction) {
            case CreatingConnection:{
                    TreeNode* endNode = findNodeAt(event->pos());
                    if (endNode && endNode != m_connectionStartNode) {
                        m_connections.append(new Connection(m_connectionStartNode, endNode));
                    }
                    break;
            }
            case DraggingNode:
                updateConnectionPositions();
                break;
            case EditingText:
                startEditingText(m_editingNode);
                update();
                break;
            } // 确保 switch 语句的括号正确关闭

        m_currentAction = None;
        m_resizeNode = m_draggingNode = nullptr;
        update();
    }
}

void CanvasWidget::keyPressEvent(QKeyEvent* event)
{
    // 处理 Ctrl+Z 退出连接模式
    if (event->key() == Qt::Key_Z && (event->modifiers() & Qt::ControlModifier)) {
        if (m_currentAction == CreatingConnection) {
            m_currentAction = None;
            update();
        }
    }
}

// === 私有辅助函数 ===
void CanvasWidget::startEditingText(TreeNode* node)
{
    if (m_textEdit) return;

    m_textEdit = new QLineEdit(this);
    m_textEdit->setText(node->text());
    m_textEdit->setAlignment(Qt::AlignCenter);

    // 设置编辑框位置
    QRect editRect = node->geometry();
    int delta1 = node->m_rect.height()/4;
    int delta2 = node->m_rect.width()/4;
    m_textEdit->setGeometry(editRect.adjusted(delta2,delta1,-delta2,-delta1));

    connect(m_textEdit, &QLineEdit::editingFinished, [=]() {
        node->setText(m_textEdit->text());
        m_textEdit->deleteLater();
        m_textEdit = nullptr;
        update();
    });

    m_textEdit->show();
    m_textEdit->setFocus();
}

void CanvasWidget::handleResize(TreeNode* node, const QPoint& mousePos)
{
    QRect newRect = node->geometry();

    // 计算坐标差并转换为 QSize
    QPoint deltaPoint = mousePos - newRect.bottomRight();
    QSize delta(deltaPoint.x(), deltaPoint.y());

    // 限制最小尺寸
    newRect.setWidth(qMax(50, newRect.width() + delta.width()));
    newRect.setHeight(qMax(30, newRect.height() + delta.height()));

    node->setGeometry(newRect);
    updateConnectionPositions();
    update();
}

void CanvasWidget::updateConnectionPositions()
{
    for (Connection* conn : qAsConst(m_connections)) {
        conn->updatePosition();
    }
}

TreeNode* CanvasWidget::findNodeAt(const QPoint& pos) const
{
    // 反向遍历实现后添加的节点在上层
    for (auto it = m_nodes.rbegin(); it != m_nodes.rend(); ++it) {
        if ((*it)->geometry().contains(pos)) {
            return *it;
        }
    }
    return nullptr;
}

// === 文件保存 ===
void CanvasWidget::saveToFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法保存文件");
        return;
    }

    QTextStream out(&file);

    // 写入节点信息
    out << "[Nodes]\n";
    for (int i = 0; i < m_nodes.size(); ++i) {
        TreeNode* node = m_nodes[i];
        QRect rect = node->geometry();
        out << QString("%1,%2,%3,%4,%5,%6\n")
                   .arg(i)                        // ID
                   .arg(rect.x()).arg(rect.y())   // 位置
                   .arg(rect.width()).arg(rect.height()) // 尺寸
                   .arg(node->text());            // 文本
    }

    // 写入连接线信息
    out << "\n[Connections]\n";
    for (Connection* conn : qAsConst(m_connections)) {
        int startID = m_nodes.indexOf(conn->startNode());
        int endID = m_nodes.indexOf(conn->endNode());
        if (startID != -1 && endID != -1) {
            out << QString("%1,%2\n").arg(startID).arg(endID);
        }
    }
}

void CanvasWidget::savetopdf(const QString& path)
{
    // 使用 QPdfWriter（更推荐）
    QPdfWriter pdfWriter(path);
    pdfWriter.setTitle("树图导出");
    pdfWriter.setCreator("矩形树图绘制器");
    pdfWriter.setResolution(96);

    // 计算所有节点的边界矩形
    QRectF boundingRect;
    for (TreeNode* node : qAsConst(m_nodes)) {
        boundingRect = boundingRect.united(node->geometry());
    }

    // 如果没有节点，使用默认大小
    if (boundingRect.isEmpty()) {
        boundingRect = QRectF(0, 0, width(), height());
    } else {
        // 添加边距
        boundingRect.adjust(-20, -20, 20, 20);
    }

    // 设置页面大小
    pdfWriter.setPageSize(QPageSize(boundingRect.size(), QPageSize::Point));
    pdfWriter.setPageMargins(QMarginsF(0, 0, 0, 0));

    QPainter painter(&pdfWriter);
    painter.setRenderHint(QPainter::Antialiasing);

    // 设置视图窗口
    painter.setWindow(boundingRect.toRect());

    // 绘制所有连接线
    for (Connection* conn : qAsConst(m_connections)) {
        conn->draw(&painter);
    }

    // 绘制所有节点
    for (TreeNode* node : qAsConst(m_nodes)) {
        node->draw(&painter, CONTROL_POINT_SIZE, PLUS_ICON_SIZE);
    }

    painter.end();
}
