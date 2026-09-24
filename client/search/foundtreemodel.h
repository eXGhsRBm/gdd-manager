#ifndef FOUNDTREEMODEL_H
#define FOUNDTREEMODEL_H

#include <QHash>
#include <QStandardItemModel>
#include "search.h"

class FoundTreeModel : public QStandardItemModel
{
    Q_OBJECT

public:
    enum Columns {
        ColFile = 0,
        ColLine,
        ColFragment,
        ColumnCount
    };

    explicit FoundTreeModel(QObject *parent = nullptr);

    void setResults(const QList<SearchMatch> &matches);
    void clearResults();

    QString pathAt(const QModelIndex &idx) const;
    int     lineAt(const QModelIndex &idx) const;

private:
    QHash<QString, QStandardItem *> m_fileItems;
};

#endif