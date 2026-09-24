#include "categorycombodelegate.h"
#include "categoriestablemodel.h"

#include <QComboBox>

CategoryComboDelegate::CategoryComboDelegate(CategoriesTableModel *model,
                                             QObject *parent)
    : QStyledItemDelegate(parent)
    , m_model(model)
{
}

QWidget *CategoryComboDelegate::createEditor(QWidget *parent,
                                             const QStyleOptionViewItem &,
                                             const QModelIndex &) const
{
    auto *cb = new QComboBox(parent);
    if (m_model)
    {
        for (const QString &id : m_model->ids())
        {
            cb->addItem(id, id);
        }
    }
    return cb;
}

void CategoryComboDelegate::setEditorData(QWidget *editor,
                                          const QModelIndex &index) const
{
    auto *cb = qobject_cast<QComboBox *>(editor);
    if (cb == nullptr)
    {
        return;
    }
    const auto current = index.data(Qt::EditRole).toString();
    const auto idx     = cb->findData(current);
    if (idx >= 0)
    {
        cb->setCurrentIndex(idx);
    }
}

void CategoryComboDelegate::setModelData(QWidget *editor,
                                         QAbstractItemModel *model,
                                         const QModelIndex &index) const
{
    auto *cb = qobject_cast<QComboBox *>(editor);
    if (cb == nullptr)
    {
        return;
    }
    model->setData(index, cb->currentData().toString(), Qt::EditRole);
}
