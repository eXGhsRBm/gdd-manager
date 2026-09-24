#ifndef EDITORSTABLEMODEL_H
#define EDITORSTABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QVector>
#include "editor.h"

class EditorsTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Columns {
        ColName = 0,
        ColPath,
        ColArgs,
        ColumnCount
    };

    explicit EditorsTableModel(QObject *parent = nullptr);

    void setHorizontalHeaderLabels(const QStringList &labels);

    QStringList horizontalHeaderLabels() const
    { return m_headers; }

    void loadFromEditors();
    void saveToEditors() const;

    void addRow();
    void addRow(const EditorData &data);
    void removeRow(int row);

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

private:
    QVector<EditorData> m_rows;
    QStringList m_headers;
};

#endif // EDITORSTABLEMODEL_H