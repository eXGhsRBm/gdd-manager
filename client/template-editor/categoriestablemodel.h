#ifndef CATEGORIESTABLEMODEL_H
#define CATEGORIESTABLEMODEL_H

#include <QAbstractTableModel>
#include <QColor>
#include <QVector>
#include "template.h"

class CategoriesTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns {
        ColId = 0,
        ColColor,
        ColumnCount
    };

    explicit CategoriesTableModel(QObject *parent = nullptr);

    void load(const QVector<CategoryInfo> &items);
    QVector<CategoryInfo> items() const;

    void addRow();
    void removeRow(int row);

    QStringList ids() const;

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
    QVector<CategoryInfo> m_rows;
	
	QStringList m_headers;
};

#endif