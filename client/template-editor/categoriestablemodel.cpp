#include "categoriestablemodel.h"
#include "translator.h"

#include <QBrush>

CategoriesTableModel::CategoriesTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers = {
        QStringLiteral("Category ID"),
        QStringLiteral("Color")
    };
}

void CategoriesTableModel::load(const QVector<CategoryInfo> &items)
{
    beginResetModel();
    m_rows = items;
    endResetModel();
}

QVector<CategoryInfo> CategoriesTableModel::items() const
{
    return m_rows;
}

QStringList CategoriesTableModel::ids() const
{
    QStringList out;
    for (const auto &r : m_rows)
    {
        if (r.id.isEmpty() == false)
        {
            out << r.id;
        }
    }
    return out;
}

void CategoriesTableModel::addRow()
{
    const auto row = m_rows.size();
    beginInsertRows({}, row, row);
    CategoryInfo c;
    c.id     = QStringLiteral("new_category");
    c.border = QColor("#6c8ebf");
    m_rows.append(c);
    endInsertRows();
}

void CategoriesTableModel::removeRow(int row)
{
    if (row < 0 || row >= m_rows.size())
    {
        return;
    }
    beginRemoveRows({}, row, row);
    m_rows.remove(row);
    endRemoveRows();
}

int CategoriesTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid()
    ? 0
    : m_rows.size();
}

int CategoriesTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid()
    ? 0
    : ColumnCount;
}

QVariant CategoriesTableModel::data(const QModelIndex &index,
                                    int role) const
{
    if (index.isValid() == false || index.row() >= m_rows.size())
    {
        return {};
    }
    const auto &row = m_rows[index.row()];

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        if (index.column() == ColId)
        {
            return row.id;
        }
        if (index.column() == ColColor)
        {
            return row.border.name();
        }
        break;
    case Qt::BackgroundRole:
        if (index.column() == ColColor)
        {
            return QBrush(row.border);
        }
        break;
    case Qt::DecorationRole:
        if (index.column() == ColColor)
        {
            return row.border;
        }
        break;
    }
    return {};
}

bool CategoriesTableModel::setData(const QModelIndex &index,
                                   const QVariant &value,
                                   int role)
{
    if (index.isValid() == false || role != Qt::EditRole)
    {
        return false;
    }
    auto &row = m_rows[index.row()];

    switch (index.column())
    {
    case ColId:
        row.id = value.toString();
        break;
    case ColColor:
    {
        const auto c = value.value<QColor>().isValid()
        ? value.value<QColor>()
        : QColor(value.toString());
        if (c.isValid())
        {
            row.border = c;
        }
        break;
    }
    default: return false;
    }

    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole, Qt::BackgroundRole, Qt::DecorationRole});
    return true;
}

Qt::ItemFlags CategoriesTableModel::flags(const QModelIndex &index) const
{
    if (index.isValid() == false)
    {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant CategoriesTableModel::headerData(int section,
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

void CategoriesTableModel::setHorizontalHeaderLabels(const QStringList &labels)
{
    if (labels.size() != ColumnCount)
    {
        return;
    }
    m_headers = labels;
    emit headerDataChanged(Qt::Horizontal, 0, ColumnCount - 1);
}