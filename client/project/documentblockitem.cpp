#include "documentblockitem.h"

#include <QBrush>
#include <QPen>
#include <QPainter>

DocumentBlockItem::DocumentBlockItem(const QString &docId,
                                     const QColor &categoryColor,
                                     const QColor &stageColor,
                                     const QColor &bgColor,
                                     QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
    , m_docId(docId)
    , m_categoryColor(categoryColor)
    , m_stageColor(stageColor)
    , m_bgColor(bgColor)
{
    setPen(QPen(Qt::darkGray, 2));
    setBrush(QBrush(m_bgColor));
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setData(0, docId);
}

void DocumentBlockItem::setExistsInProject(bool exists)
{
    m_exists = exists;
    const auto c = exists
                       ? m_bgColor
                       : m_bgColor.darker(115);
    setBrush(QBrush(c));
    update();
}

void DocumentBlockItem::paint(QPainter *painter,
                              const QStyleOptionGraphicsItem *option,
                              QWidget *widget)
{
    QGraphicsRectItem::paint(painter, option, widget);

    const auto r = rect().adjusted(3, 3, -3, -3);

    painter->setPen(QPen(m_stageColor, 4, Qt::SolidLine, Qt::RoundCap));
    painter->drawLine(r.topLeft(), r.topRight());

    painter->setPen(QPen(m_categoryColor, 4, Qt::SolidLine, Qt::RoundCap));
    painter->drawLine(r.topLeft(), r.bottomLeft());
}