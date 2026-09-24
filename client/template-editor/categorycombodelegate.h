#ifndef CATEGORYCOMBODELEGATE_H
#define CATEGORYCOMBODELEGATE_H

#include <QStyledItemDelegate>

class CategoriesTableModel;

class CategoryComboDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit CategoryComboDelegate(CategoriesTableModel *model,
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
    CategoriesTableModel *m_model;
};

#endif // CATEGORYCOMBODELEGATE_H
