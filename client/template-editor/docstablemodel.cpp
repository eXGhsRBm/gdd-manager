#include "docstablemodel.h"

DocsTableModel::DocsTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers = {
        QStringLiteral("Document ID"),
        QStringLiteral("Category"),
        QStringLiteral("Stage")
    };
}

void DocsTableModel::load(const QVector<DocumentInfo> &items)
{
    beginResetModel();
    m_rows = items;
    endResetModel();
}

QVector<DocumentInfo> DocsTableModel::items() const
{
    return m_rows;
}

void DocsTableModel::addRow()
{
    const auto row = m_rows.size();
    beginInsertRows({}, row, row);
    DocumentInfo d;
    d.id       = QStringLiteral("new_document");
    d.category = QString();
    d.stage    = QString();
    m_rows.append(d);
    endInsertRows();
}

void DocsTableModel::removeRow(int row)
{
    if (row < 0 || row >= m_rows.size())
    {
        return;
    }
    beginRemoveRows({}, row, row);
    m_rows.remove(row);
    endRemoveRows();
}

int DocsTableModel::countByCategory(const QString &categoryId) const
{
    int n = 0;
    for (const auto &r : m_rows)
    {
        if (r.category == categoryId)
        {
            ++n;
        }
    }
    return n;
}

int DocsTableModel::countByStage(const QString &stageId) const
{
    int n = 0;
    for (const auto &r : m_rows)
    {
        if (r.stage == stageId)
        {
            ++n;
        }
    }
    return n;
}

int DocsTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid()
    ? 0
    : m_rows.size();
}

int DocsTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid()
    ? 0
    : ColumnCount;
}

QVariant DocsTableModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() == false || index.row() >= m_rows.size())
    {
        return {};
    }
    const auto &row = m_rows[index.row()];

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
        case ColId:       return row.id;
        case ColCategory: return row.category;
        case ColStage:    return row.stage;
        }
    }
    return {};
}

bool DocsTableModel::setData(const QModelIndex &index,
                             const QVariant &value, int role)
{
    if (index.isValid() == false || role != Qt::EditRole)
    {
        return false;
    }
    auto &row = m_rows[index.row()];

    switch (index.column())
    {
    case ColId:       row.id       = value.toString(); break;
    case ColCategory: row.category = value.toString(); break;
    case ColStage:    row.stage    = value.toString(); break;
    default: return false;
    }

    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    return true;
}

Qt::ItemFlags DocsTableModel::flags(const QModelIndex &index) const
{
    if (index.isValid() == false)
    {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant DocsTableModel::headerData(int section,
                                    Qt::Orientation o,
                                    int role) const
{
    if (role != Qt::DisplayRole || o != Qt::Horizontal)
    {
        return {};
    }
    if (section < 0 || section >= m_headers.size())
    {
        return {};
    }
    return m_headers.at(section);
}

void DocsTableModel::setHorizontalHeaderLabels(const QStringList &labels)
{
    if (labels.size() != ColumnCount)
    {
        return;
    }
    m_headers = labels;
    emit headerDataChanged(Qt::Horizontal, 0, ColumnCount - 1);
}