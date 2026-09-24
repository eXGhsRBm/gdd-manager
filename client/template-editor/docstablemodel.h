#ifndef DOCSTABLEMODEL_H
#define DOCSTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include "template.h"

class DocsTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Columns {
        ColId = 0,
        ColCategory,
        ColStage,
        ColumnCount
    };

    explicit DocsTableModel(QObject *parent = nullptr);

    void load(const QVector<DocumentInfo> &items);
    QVector<DocumentInfo> items() const;

    void addRow();
    void removeRow(int row);

    int countByCategory(const QString &categoryId) const;
    int countByStage(const QString &stageId) const;

    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index,
                  int role) const override;
    bool setData(const QModelIndex &index,
                 const QVariant &value,
                 int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section,
                        Qt::Orientation o,
                        int role) const override;

    void setHorizontalHeaderLabels(const QStringList &labels);

private:
    QVector<DocumentInfo> m_rows;

    QStringList m_headers;
};

#endif