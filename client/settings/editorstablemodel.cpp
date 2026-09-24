#include "editorstablemodel.h"

EditorsTableModel::EditorsTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers = { QStringLiteral("Name"),
                 QStringLiteral("Path"),
                 QStringLiteral("Arguments") };
}


void EditorsTableModel::loadFromEditors()
{
    beginResetModel();
    m_rows.clear();

    auto &editor = Editor::instance();
    for (const QString &name : editor.names())
    {
        m_rows.append(editor.get(name));
    }
    endResetModel();
}

void EditorsTableModel::saveToEditors() const
{
    auto &editor = Editor::instance();
    editor.clear();
    for (const auto &row : m_rows)
    {
        if (row.name.isEmpty())
        {
            continue;
        }
        editor.set(row.name, row);
    }
}


void EditorsTableModel::addRow()
{
    EditorData e;
    e.name = QStringLiteral("New Editor");
    e.path = QString();
    e.args = QStringLiteral("{file}:{line}");
    addRow(e);
}

void EditorsTableModel::addRow(const EditorData &data)
{
    const auto row = m_rows.size();
    beginInsertRows({}, row, row);
    m_rows.append(data);
    endInsertRows();
}

void EditorsTableModel::removeRow(int row)
{
    if (row < 0 || row >= m_rows.size())
    {
        return;
    }

    beginRemoveRows({}, row, row);
    m_rows.remove(row);
    endRemoveRows();
}

int EditorsTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid()
               ? 0
               : m_rows.size();
}

int EditorsTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid()
               ? 0
               : ColumnCount;
}

QVariant EditorsTableModel::data(const QModelIndex &index, int role) const
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
        case ColName: return row.name;
        case ColPath: return row.path;
        case ColArgs: return row.args;
        }
    }
    return {};
}

bool EditorsTableModel::setData(const QModelIndex &index,
                                const QVariant &value, int role)
{
    if (index.isValid() == false || role != Qt::EditRole)
    {
        return false;
    }

    if (index.row() >= m_rows.size())
    {
        return false;
    }
    auto &row = m_rows[index.row()];

    const auto v = value.toString();
    switch (index.column())
    {
    case ColName: row.name = v; break;
    case ColPath: row.path = v; break;
    case ColArgs: row.args = v; break;
    default: return false;
    }
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    return true;
}

Qt::ItemFlags EditorsTableModel::flags(const QModelIndex &index) const
{
    if (index.isValid() == false)
    {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void EditorsTableModel::setHorizontalHeaderLabels(const QStringList &labels)
{
    if (labels.size() != ColumnCount)
    {
        return;
    }
    m_headers = labels;

    emit headerDataChanged(Qt::Horizontal, 0, ColumnCount - 1);
}

QVariant EditorsTableModel::headerData(int section,
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