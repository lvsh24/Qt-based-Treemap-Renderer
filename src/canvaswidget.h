#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QList>

// 前向声明（避免头文件循环依赖）
class TreeNode;
class Connection;

/**
 * @brief 核心画布组件，负责所有图形元素的绘制和交互逻辑
 *
 * 继承自 QWidget，通过重写 paintEvent 和事件处理器实现自定义绘图
 */
class CanvasWidget : public QWidget
{
    Q_OBJECT

public:
    // 操作类型枚举
    enum ActionType {
        None,           // 无操作
        Resizing,       // 正在调整矩形大小
        DraggingNode,   // 正在拖拽矩形
        CreatingConnection, // 正在创建连接线
        EditingText //正在编辑文本
    };

    explicit CanvasWidget(QWidget *parent = nullptr);
    ~CanvasWidget() override;

    // 提供给 MainWindow 的公共接口
    void clear();  // 清空所有元素
    TreeNode* addTreeNode(const QRect &rect, const QString &text);// 添加新节点
    void addConnection(TreeNode *start,TreeNode *end);
    void saveToFile(const QString &path); // 保存到文件
    void savetopdf(const QString &path);
    const QList<TreeNode*>& nodes() const { return m_nodes; } // 提供给 Connection 访问节点列表

protected:
    // 重写 Qt 事件处理函数
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    // 私有辅助函数
    void startEditingText(TreeNode *node); // 启动文本编辑
    void handleResize(TreeNode *node, const QPoint &mousePos); // 处理调整大小
    void updateConnectionPositions(); // 更新所有连接线位置
    TreeNode* findNodeAt(const QPoint &pos) const; // 查找坐标处的节点

    // 图形元素存储
    QList<TreeNode*> m_nodes;          // 所有矩形节点
    QList<Connection*> m_connections;  // 所有连接线

    // 交互状态管理
    ActionType m_currentAction = None; // 当前操作类型
    TreeNode* m_resizeNode = nullptr;  // 正在调整大小的节点
    TreeNode* m_draggingNode = nullptr;// 正在拖拽的节点
    TreeNode* m_editingNode = nullptr; // 正在编辑文本的节点
    QLineEdit* m_textEdit = nullptr;   // 文本编辑框

    // 连接线创建相关
    TreeNode* m_connectionStartNode = nullptr; // 连接线起点节点
    QPoint m_tempConnectionEnd;        // 临时连接线终点（鼠标位置）

    // 几何计算辅助
    QPoint m_dragStartPos;             // 拖拽起始坐标
    QPoint m_nodeDragStartPos;         // 节点拖拽起始位置

    // 控制点尺寸常量
    static const int CONTROL_POINT_SIZE = 8; // 调整大小控制点边长
    static const int PLUS_ICON_SIZE = 12;    // 加号图标边长
};
