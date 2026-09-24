#ifndef GENERATORTABLEMODEL_H
#define GENERATORTABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QVector>

#include "template.h"

struct GeneratorRow {
    QString docId;
    QStringList tocFiles;
    QString selectedToc;
};

class GeneratorTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Columns {
        ColDocument = 0,
        ColToc      = 1,
        ColumnCount = 2
    };

    explicit GeneratorTableModel(QObject *parent = nullptr);

    void loadFromCatalog(const QVector<DocumentInfo> &docs,
                         const QString &tocDir);

    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation o,
                        int role) const override;

    QStringList tocFilesForRow(int row) const;
    QString     selectedTocForRow(int row) const;
    QString     docIdForRow(int row) const;

    QStringList selectedTocs() const;

    void setHorizontalHeaderLabels(const QStringList &labels);

private:
    QVector<GeneratorRow> m_rows;

    QStringList m_headers;
};

#endif // GENERATORTABLEMODEL_H