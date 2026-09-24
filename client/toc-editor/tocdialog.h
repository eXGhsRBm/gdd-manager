#ifndef TOCDIALOG_H
#define TOCDIALOG_H

#include "translator.h"

#include <QDialog>

class SectionsListModel;

namespace Ui { class TocDialog; }

class TocDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TocDialog(QWidget *parent = nullptr);
    ~TocDialog();

private slots:
    void on_pushButton_addToc_clicked();
    void on_pushButton_addSection_clicked();
    void on_pushButton_save_clicked();

    void on_comboBox_tocs_currentIndexChanged(int index);
    void onSectionsContextMenu(const QPoint &pos);

    void on_pushButton_resetSectionChanges_clicked();

private:
    Ui::TocDialog *ui;
    SectionsListModel *m_sectionsModel = nullptr;

    QStringList m_savedSections;
    bool        m_isSample = false;

    void translation();
    void populateTocs();
    void loadTocFrom(const QString &path);
    bool saveToc(const QString &path);

    bool isSample(const QString &fileName) const;
    void updateActionsState();

    QString tocDir() const;
    QString selectedTocPath() const;
    QString selectedTocId() const;
};

#endif