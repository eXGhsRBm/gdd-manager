#ifndef STAGESTABLEMODEL_H
#define STAGESTABLEMODEL_H

#include <QAbstractTableModel>
#include <QColor>
#include <QVector>
#include "template.h"

class StagesTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Columns {
        ColId = 0,
        ColColor,
        ColumnCount
    };

    explicit StagesTableModel(QObject *parent = nullptr);

    void load(const QVector<StageInfo> &items);
    QVector<StageInfo> items() const;

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
    QVector<StageInfo> m_rows;
	
	QStringList m_headers;
};


#endif // STAGESTABLEMODEL_H
