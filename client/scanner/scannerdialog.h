#ifndef SCANNERDIALOG_H
#define SCANNERDIALOG_H

#include "translator.h"

#include <QDialog>

class IssuesTreeModel;

namespace Ui { class ScannerDialog; }

class ScannerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScannerDialog(QWidget *parent = nullptr);
    ~ScannerDialog();

    void setProjectDir(const QString &dir)
    { m_projectDir = dir; }

    void setProjectJsonPath(const QString &path)
    { m_projectJsonPath = path; }

private slots:
    void on_pushButton_scan_clicked();
    void onIssueDoubleClicked(const QModelIndex &idx);

private:
    Ui::ScannerDialog *ui;
    IssuesTreeModel *m_model = nullptr;
    QString m_projectDir;
    QString m_projectJsonPath;

    void translation();
    void setupTree();
    void openInEditor(const QString &path,
                      int line);
};

#endif