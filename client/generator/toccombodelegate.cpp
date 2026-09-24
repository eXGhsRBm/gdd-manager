#include "toccombodelegate.h"
#include "generatortablemodel.h"

#include <QComboBox>
#include <QFileInfo>

TocComboDelegate::TocComboDelegate(GeneratorTableModel *model,
                                   QObject *parent)
    : QStyledItemDelegate(parent)
    , m_model(model)
{
}

QWidget *TocComboDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &,
                                        const QModelIndex &index) const
{
    if (index.column() != GeneratorTableModel::ColToc)
    {
        return nullptr;
    }

    auto *combo = new QComboBox(parent);
    const auto files = m_model->tocFilesForRow(index.row());

    for (const auto &path : files)
    {
        const auto name = QFileInfo(path).fileName();
        combo->addItem(name, path);
    }
    return combo;
}

void TocComboDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{
    auto *combo = qobject_cast<QComboBox *>(editor);
    if (combo == nullptr)
    {
        return;
    }

    const auto current = m_model->selectedTocForRow(index.row());
    const auto idx     = combo->findData(current);
    if (idx >= 0)
    {
        combo->setCurrentIndex(idx);
    }
}

void TocComboDelegate::setModelData(QWidget *editor,
                                    QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    auto *combo = qobject_cast<QComboBox *>(editor);
    if (combo == nullptr)
    {
        return;
    }

    const auto path = combo->currentData().toString();
    model->setData(index, path, Qt::EditRole);
}