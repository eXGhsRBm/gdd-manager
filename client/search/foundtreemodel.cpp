#include "foundtreemodel.h"

#include <QFileInfo>
#include <QStandardItem>

FoundTreeModel::FoundTreeModel(QObject *parent)
    : QStandardItemModel(parent)
{
    setColumnCount(ColumnCount);

}

void FoundTreeModel::clearResults()
{
    removeRows(0, rowCount());
    m_fileItems.clear();
}

void FoundTreeModel::setResults(const QList<SearchMatch> &matches)
{
    clearResults();

    for (const auto &m : matches)
    {
        auto parent = m_fileItems.value(m.path, nullptr);
        if (parent == nullptr)
        {
            parent = new QStandardItem(m.name);
            parent->setEditable(false);
            parent->setToolTip(m.path);
            parent->setData(m.path, Qt::UserRole);
            parent->setData(0,      Qt::UserRole + 1);

            QIcon icon = m.isFolder
                             ? QIcon::fromTheme("folder")
                             : QIcon::fromTheme("text-x-generic");
            parent->setIcon(icon);

            appendRow({ parent, new QStandardItem(), new QStandardItem() });
            m_fileItems.insert(m.path, parent);
        }

        if (m.line > 0)
        {
            auto *lineItem = new QStandardItem(QString::number(m.line));
            lineItem->setEditable(false);
            lineItem->setData(m.path, Qt::UserRole);
            lineItem->setData(m.line, Qt::UserRole + 1);

            auto *previewItem = new QStandardItem(m.preview);
            previewItem->setEditable(false);
            previewItem->setToolTip(m.preview);

            parent->appendRow({
                new QStandardItem(),
                lineItem,
                previewItem
            });
        }
    }
}

QString FoundTreeModel::pathAt(const QModelIndex &idx) const
{
    if (idx.isValid() == false)
    {
        return {};
    }

    if (auto item = itemFromIndex(idx))
    {
        const auto p = item->data(Qt::UserRole).toString();
        if (p.isEmpty() == false)
        {
            return p;
        }
    }

    for (auto c = 0; c < columnCount(); ++c)
    {
        if (c == idx.column())
        {
            continue;
        }

        if (auto sib = itemFromIndex(idx.sibling(idx.row(), c)))
        {
            const auto p = sib->data(Qt::UserRole).toString();
            if (p.isEmpty() == false)
            {
                return p;
            }
        }
    }
    return {};
}

int FoundTreeModel::lineAt(const QModelIndex &idx) const
{
    if (idx.isValid() == false)
    {
        return 0;
    }

    if (auto item = itemFromIndex(idx))
    {
        const auto n = item->data(Qt::UserRole + 1).toInt();
        if (n > 0)
        {
            return n;
        }
    }

    for (auto c = 0; c < columnCount(); ++c)
    {
        if (c == idx.column())
        {
            continue;
        }

        if (auto sib = itemFromIndex(idx.sibling(idx.row(), c)))
        {
            const auto n = sib->data(Qt::UserRole + 1).toInt();
            if (n > 0)
            {
                return n;
            }
        }
    }
    return 0;
}