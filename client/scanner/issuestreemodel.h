#ifndef ISSUESTREEMODEL_H
#define ISSUESTREEMODEL_H

#include <QHash>
#include <QStandardItemModel>
#include "scanner.h"

class IssuesTreeModel : public QStandardItemModel
{
    Q_OBJECT

public:
    enum Columns {
        ColFile = 0,
        ColError,
        ColDetails,
        ColLine,
        ColumnCount
    };

    explicit IssuesTreeModel(QObject *parent = nullptr);

    void setIssues(const QList<ScanIssue> &issues);
    void clearIssues();

    QString pathAt(const QModelIndex &idx) const;
    int     lineAt(const QModelIndex &idx) const;

private:
    QHash<QString, QStandardItem *> m_fileItems;
};

#endif