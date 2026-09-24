#ifndef SECTIONSLISTMODEL_H
#define SECTIONSLISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class SectionsListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SectionsListModel(QObject *parent = nullptr);

    void load(const QStringList &sections);

    QStringList sections() const;

    void addRow();
    void removeRow(int row);

    int rowCount(const QModelIndex &parent = {}) const override;

    QVariant data(const QModelIndex &index,
                  int role) const override;

    bool setData(const QModelIndex &index,
                 const QVariant &value,
                 int role) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    QStringList m_rows;
};

#endif