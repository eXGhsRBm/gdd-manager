#ifndef CATEGORYCOLORBUTTONDELEGATE_H
#define CATEGORYCOLORBUTTONDELEGATE_H

#include <QStyledItemDelegate>

class CategoryColorButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit CategoryColorButtonDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    bool editorEvent(QEvent *event,
                     QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;
};

#endif