#include "stagecolorbuttondelegate.h"

#include <QColorDialog>
#include <QPainter>
#include <QMouseEvent>

StageColorButtonDelegate::StageColorButtonDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void StageColorButtonDelegate::paint(QPainter *painter,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    const auto c = index.data(Qt::DecorationRole).value<QColor>();
    if (c.isValid() == false)
    {
        return;
    }

    const QRect r = option.rect.adjusted(4, 4, -4, -4);
    painter->save();
    painter->setBrush(c);
    painter->setPen(QPen(Qt::darkGray, 1));
    painter->drawRect(r);
    painter->restore();
}

bool StageColorButtonDelegate::editorEvent(QEvent *event,
                                           QAbstractItemModel *model,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonDblClick)
    {
        const auto current = index.data(Qt::DecorationRole).value<QColor>();
        const auto chosen  = QColorDialog::getColor(current.isValid()
                                                         ? current
                                                         : QColor("#000000"));
        if (chosen.isValid())
        {
            model->setData(index, chosen, Qt::EditRole);
            return true;
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}