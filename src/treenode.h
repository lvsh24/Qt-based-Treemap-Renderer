#pragma once

#include <QRect>
#include <QString>
#include <QPoint>
#include <QPainter>
#include <QColor>

static const int TEXT_MARGIN = 8;

/**
 * @brief 表示单个矩形树图节点
 *
 * 封装节点的几何属性、文本内容和绘制逻辑
 * 提供对节点状态（悬停/调整大小）的检测支持
 */
class CanvasWidget;

class TreeNode
{
public:
    /**
     * @brief 构造函数
     * @param rect 初始几何位置和尺寸
     * @param text 显示文本内容
     */
    // 修改构造函数以接收 CanvasWidget 指针
    explicit TreeNode(CanvasWidget* canvas, const QRect& rect, const QString& text = "");

    // 添加 canvas() 访问器
    CanvasWidget* canvas() const { return m_canvas; }

    // 基础属性访问器
    QRect geometry() const    { return m_rect; }      ///< 获取节点几何属性
    QString text() const      { return m_text; }      ///< 获取显示文本
    QPoint center() const;                            ///< 计算矩形中心点

    // 状态设置
    void setGeometry(const QRect& rect) { m_rect = rect; } ///< 设置节点位置和尺寸
    void setText(const QString& text)   { m_text = text; } ///< 设置显示文本
    void setHovered(bool hovered)       { m_hovered = hovered; } ///< 设置悬停状态

    /**
     * @brief 检测点是否在节点区域内
     * @param point 检测坐标点
     * @return 是否包含该点
     */
    bool containsPoint(const QPoint& point) const;

    /**
     * @brief 执行节点绘制
     * @param painter 绘图设备
     * @param controlSize 调整控制点尺寸（像素）
     * @param plusSize 加号图标尺寸（像素）
     */
    void draw(QPainter* painter, int controlSize, int plusSize) const;

    /**
     * @brief 移动节点到指定位置（保持尺寸不变）
     * @param topLeft 新的左上角坐标
     */
    void moveTo(const QPoint& topLeft);

private:
    CanvasWidget* m_canvas;
    QRect m_rect;        ///< 节点几何属性（位置+尺寸）
    QString m_text;      ///< 显示文本内容
    bool m_hovered = false; ///< 是否处于悬停状态

    // 样式常量（可在实现中修改）
    static const QColor FILL_COLOR;      ///< 默认填充色
    static const QColor BORDER_COLOR;    ///< 边框颜色
    static const QColor TEXT_COLOR;      ///< 文本颜色

    friend class CanvasWidget;
};
