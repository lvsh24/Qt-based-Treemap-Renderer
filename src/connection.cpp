#include "connection.h"
#include "treenode.h"
#include "canvaswidget.h"
#include <QPainter>
#include <QLineF>
#include <QRectF>

// 样式常量定义
const QColor Connection::LINE_COLOR = Qt::darkGray;
const double Connection::LINE_WIDTH = 2.0;
const Qt::PenStyle Connection::LINE_STYLE = Qt::SolidLine;

Connection::Connection(TreeNode* start, TreeNode* end)
    : m_startNode(start), m_endNode(end)
{
    updatePosition();
}

void Connection::updatePosition()
{
    m_line = QLineF(m_startNode->center(), m_endNode->center());
}

void Connection::draw(QPainter* painter) const
{
    painter->save();

    // 设置线段样式
    QPen pen(LINE_COLOR, LINE_WIDTH);
    pen.setStyle(LINE_STYLE);
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);

    // 绘制线段
    painter->drawLine(m_line);
    painter->restore();
}
