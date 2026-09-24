#include "issuestreemodel.h"
#include "translation.h"

#include <QFileInfo>
#include <QStandardItem>

IssuesTreeModel::IssuesTreeModel(QObject *parent)
    : QStandardItemModel(parent)
{
    setColumnCount(ColumnCount);
}

void IssuesTreeModel::clearIssues()
{
    removeRows(0, rowCount());
    m_fileItems.clear();
}

void IssuesTreeModel::setIssues(const QList<ScanIssue> &issues)
{
    clearIssues();

    for (const auto &i : issues)
    {
        auto parent = m_fileItems.value(i.path, nullptr);
        if (parent == nullptr)
        {
            parent = new QStandardItem(QFileInfo(i.path).fileName());
            parent->setEditable(false);
            parent->setToolTip(i.path);
            parent->setData(i.path, Qt::UserRole);

            appendRow({
                parent,
                new QStandardItem(),   // Error
                new QStandardItem(),   // Details
                new QStandardItem()    // Line
            });
            m_fileItems.insert(i.path, parent);
        }

        auto *errItem = new QStandardItem(TRANSLATE_ERR(i.errorId));
        errItem->setEditable(false);

        auto *detItem = new QStandardItem(i.details);
        detItem->setEditable(false);
        detItem->setToolTip(i.details);

        auto *lineItem = new QStandardItem(i.line > 0
                                               ? QString::number(i.line)
                                               : QString());
        lineItem->setEditable(false);
        lineItem->setData(i.path, Qt::UserRole);
        lineItem->setData(i.line, Qt::UserRole + 1);

        parent->appendRow({
            new QStandardItem(),
            errItem,
            detItem,
            lineItem
        });
    }
}

QString IssuesTreeModel::pathAt(const QModelIndex &idx) const
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

int IssuesTreeModel::lineAt(const QModelIndex &idx) const
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