#include "treenode.h"
#include "canvaswidget.h"
#include <QPainter>
#include <QFontMetrics>

// 样式常量初始化
const QColor TreeNode::FILL_COLOR = QColor(245, 245, 245);    // 浅灰色填充
const QColor TreeNode::BORDER_COLOR = Qt::darkGray;           // 深灰边框
const QColor TreeNode::TEXT_COLOR = Qt::black;                 // 黑色文本

TreeNode::TreeNode(CanvasWidget* canvas, const QRect& rect, const QString& text)
    : m_canvas(canvas), m_rect(rect), m_text(text)
{
}

QPoint TreeNode::center() const
{
    return m_rect.center();
}

bool TreeNode::containsPoint(const QPoint& point) const
{
    return m_rect.contains(point);
}

void TreeNode::draw(QPainter* painter, int controlSize, int plusSize) const
{
    // === 绘制主体矩形 ===
    painter->save();

    // 设置填充和边框
    painter->setBrush(FILL_COLOR);
    painter->setPen(QPen(BORDER_COLOR, 1));
    painter->drawRect(m_rect);

    // === 绘制文本 ===
    painter->setPen(TEXT_COLOR);
    QFont font = painter->font();

    // 计算可用的文本区域（考虑边距）
    QRect textRect = m_rect.adjusted(TEXT_MARGIN, TEXT_MARGIN, -TEXT_MARGIN, -TEXT_MARGIN);

    // 自动调整字体大小以适应矩形
    int fontSize = 12;
    QFontMetrics metrics(font);
    while (fontSize > 8 && metrics.height() > textRect.height()/2) {
        fontSize--;
        font.setPointSize(fontSize);
        metrics = QFontMetrics(font);
    }
    painter->setFont(font);

    // === 绘制交互元素 ===
    if (m_hovered) {
        // 绘制右下角调整控制点
        QRect resizeHandle(m_rect.right() - controlSize,
                           m_rect.bottom() - controlSize,
                           controlSize * 2, controlSize * 2);
        painter->setBrush(Qt::white);
        painter->drawRect(resizeHandle);

        // 绘制右侧加号图标
        QRect plusIcon(m_rect.right() - plusSize,
                       m_rect.center().y() - plusSize/2,
                       plusSize, plusSize);
        painter->setPen(QPen(Qt::darkGray, 1.5));
        painter->drawLine(plusIcon.left() + 2, plusIcon.center().y(),
                          plusIcon.right() - 2, plusIcon.center().y());
        painter->drawLine(plusIcon.center().x(), plusIcon.top() + 2,
                          plusIcon.center().x(), plusIcon.bottom() - 2);

        // 绘制中心文本编辑框
        int delta1 = m_rect.height()/4;
        int delta2 = m_rect.width()/4;
        QRect textIcon = m_rect.adjusted(delta2,delta1,-delta2,-delta1);
        painter->setBrush(Qt::white);
        painter->drawRect(textIcon);
    }

    // 绘制居中文本（自动换行+省略号）
    painter->drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap,
                      metrics.elidedText(m_text, Qt::ElideRight, textRect.width()));

    painter->restore();
}

void TreeNode::moveTo(const QPoint& topLeft)
{
    m_rect.moveTo(topLeft);
}
