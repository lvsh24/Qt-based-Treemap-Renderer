#pragma once

#include <QLineF>
#include <QList>
#include <QPainter>

class TreeNode; // 前向声明

/**
 * @brief 表示两个矩形节点之间的连接线
 *
 * 负责管理连接线的几何计算和绘制逻辑，自动处理线段裁剪
 */
class Connection
{
public:
    /**
     * @brief 构造函数
     * @param start 起始节点
     * @param end 目标节点
     */
    explicit Connection(TreeNode* start, TreeNode* end);

    // 基础属性访问器
    TreeNode* startNode() const { return m_startNode; } ///< 获取起始节点
    TreeNode* endNode() const   { return m_endNode; }   ///< 获取目标节点

    /**
     * @brief 更新连接线路径（当节点移动时调用）
     */
    void updatePosition();

    /**
     * @brief 执行线段绘制
     * @param painter 绘图设备
     */
    void draw(QPainter* painter) const;

private:
    // 线段裁剪算法
    QLineF calculateVisibleSegment() const;

    TreeNode* m_startNode; ///< 起始节点（不可为nullptr）
    TreeNode* m_endNode;   ///< 目标节点（不可为nullptr）
    QLineF m_line;         ///< 当前连接线几何路径

    // 样式常量
    static const QColor LINE_COLOR;       ///< 线段颜色
    static const double LINE_WIDTH;       ///< 线宽
    static const Qt::PenStyle LINE_STYLE; ///< 线型
};
