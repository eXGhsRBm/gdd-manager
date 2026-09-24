#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include "translator.h"

#include <QDialog>

class FoundTreeModel;

namespace Ui { class SearchDialog; }

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = nullptr);
    ~SearchDialog();

    void setProjectDir(const QString &dir)
    { m_projectDir = dir; }

private slots:
    void on_pushButton_search_clicked();
    void onResultDoubleClicked(const QModelIndex &idx);

private:
    Ui::SearchDialog *ui;
    FoundTreeModel *m_model = nullptr;
    QString m_projectDir;

    void translation();
    void setupTree();
    void openInEditor(const QString &path,
                      int line);
};

#endif