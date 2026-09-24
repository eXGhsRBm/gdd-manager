#ifndef STAGECOLORBUTTONDELEGATE_H
#define STAGECOLORBUTTONDELEGATE_H

#include <QStyledItemDelegate>

class StageColorButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit StageColorButtonDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    bool editorEvent(QEvent *event,
                     QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;
};

#endif // STAGECOLORBUTTONDELEGATE_H
