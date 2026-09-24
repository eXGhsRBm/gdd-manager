#include "sectionslistmodel.h"

SectionsListModel::SectionsListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void SectionsListModel::load(const QStringList &sections)
{
    beginResetModel();
    m_rows = sections;
    endResetModel();
}

QStringList SectionsListModel::sections() const
{
    return m_rows;
}

void SectionsListModel::addRow()
{
    const auto row = m_rows.size();
    beginInsertRows({}, row, row);
    m_rows.append(QStringLiteral("new_section"));
    endInsertRows();
}

void SectionsListModel::removeRow(int row)
{
    if (row < 0 || row >= m_rows.size())
    {
        return;
    }

    beginRemoveRows({}, row, row);
    m_rows.removeAt(row);
    endRemoveRows();
}

int SectionsListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid()
               ? 0
               : m_rows.size();
}

QVariant SectionsListModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() == false || index.row() >= m_rows.size())
    {
        return {};
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        return m_rows[index.row()];
    }

    return {};
}

bool SectionsListModel::setData(const QModelIndex &index,
                                const QVariant &value,
                                int role)
{
    if (index.isValid() == false || role != Qt::EditRole)
    {
        return false;
    }

    if (index.row() >= m_rows.size())
    {
        return false;
    }

    m_rows[index.row()] = value.toString();
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    return true;
}

Qt::ItemFlags SectionsListModel::flags(const QModelIndex &index) const
{
    if (index.isValid() == false)
    {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}