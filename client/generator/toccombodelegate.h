#ifndef TOCCOMBODELEGATE_H
#define TOCCOMBODELEGATE_H

#include <QStyledItemDelegate>

class GeneratorTableModel;

class TocComboDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit TocComboDelegate(GeneratorTableModel *model,
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
    GeneratorTableModel *m_model;
};

#endif // TOCCOMBODELEGATE_H