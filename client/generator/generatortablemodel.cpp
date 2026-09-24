#include "generatortablemodel.h"
#include "loader.h"

#include "settings.h"

#include <QBrush>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QJsonObject>

GeneratorTableModel::GeneratorTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers = {
        QStringLiteral("Document"),
        QStringLiteral("TOC")
    };
}

void GeneratorTableModel::loadFromCatalog(const QVector<DocumentInfo> &docs,
                                          const QString &tocDir)
{
    beginResetModel();
    m_rows.clear();

    QMap<QString, QStringList> tocById;
    QDirIterator it(tocDir, {QStringLiteral("*.json")}, QDir::Files);
    while (it.hasNext())
    {
        const auto path = it.next();
        const auto root = Loader::load(path);
        const auto id   = root.value(QStringLiteral("id")).toString();

        if (id.isEmpty() == false)
        {
            tocById[id].append(path);
        }
    }

    for (const auto &d : docs)
    {
        GeneratorRow row;
        row.docId    = d.id;
        row.tocFiles = tocById.value(d.id);

        if (row.tocFiles.isEmpty() == false)
        {
            row.selectedToc = row.tocFiles.first();
        }

        m_rows.append(row);
    }

    endResetModel();
}

int GeneratorTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid()
    ? 0
    : m_rows.size();
}

int GeneratorTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid()
    ? 0
    : ColumnCount;
}

QVariant GeneratorTableModel::data(const QModelIndex &index, int role) const
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
        switch (index.column())
        {
        case ColDocument:
            return row.docId;
        case ColToc:
            return row.selectedToc.isEmpty()
                       ? QString()
                       : QFileInfo(row.selectedToc).fileName();
        }
        break;

    case Qt::ToolTipRole:
        if (index.column() == ColToc)
        {
            return row.selectedToc;
        }
        break;

    case Qt::ForegroundRole:
        if (row.tocFiles.isEmpty())
        {
            return QBrush(Qt::red);
        }
        break;
    }

    return {};
}

bool GeneratorTableModel::setData(const QModelIndex &index,
                                  const QVariant &value,
                                  int role)
{
    if (index.isValid() == false || role != Qt::EditRole)
    {
        return false;
    }

    if (index.column() != ColToc)
    {
        return false;
    }

    const auto path = value.toString();
    if (m_rows[index.row()].tocFiles.contains(path) == false)
    {
        return false;
    }

    m_rows[index.row()].selectedToc = path;
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    return true;
}

Qt::ItemFlags GeneratorTableModel::flags(const QModelIndex &index) const
{
    if (index.isValid() == false)
    {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.column() == ColToc && m_rows[index.row()].tocFiles.isEmpty() == false)
    {
        f |= Qt::ItemIsEditable;
    }
    return f;
}

void GeneratorTableModel::setHorizontalHeaderLabels(const QStringList &labels)
{
    if (labels.size() != ColumnCount)
    {
        return;
    }
    m_headers = labels;
    emit headerDataChanged(Qt::Horizontal, 0, ColumnCount - 1);
}

QVariant GeneratorTableModel::headerData(int section, Qt::Orientation o, int role) const
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

QStringList GeneratorTableModel::tocFilesForRow(int row) const
{
    if (row < 0 || row >= m_rows.size())
    {
        return {};
    }
    return m_rows[row].tocFiles;
}

QString GeneratorTableModel::selectedTocForRow(int row) const
{
    if (row < 0 || row >= m_rows.size())
    {
        return {};
    }
    return m_rows[row].selectedToc;
}

QString GeneratorTableModel::docIdForRow(int row) const
{
    if (row < 0 || row >= m_rows.size())
    {
        return {};
    }
    return m_rows[row].docId;
}

QStringList GeneratorTableModel::selectedTocs() const
{
    QStringList out;
    for (const auto &r : m_rows)
    {
        if (r.selectedToc.isEmpty() == false)
        {
            out << r.selectedToc;
        }
    }
    return out;
}