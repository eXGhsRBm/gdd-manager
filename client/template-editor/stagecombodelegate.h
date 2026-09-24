#ifndef STAGECOMBODELEGATE_H
#define STAGECOMBODELEGATE_H

#include <QStyledItemDelegate>

class StagesTableModel;

class StageComboDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit StageComboDelegate(StagesTableModel *model,
                                QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const override;

    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override;

private:
    StagesTableModel *m_model;
};

#endif // STAGECOMBODELEGATE_H
