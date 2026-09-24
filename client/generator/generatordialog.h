#ifndef GENERATORDIALOG_H
#define GENERATORDIALOG_H

#include "translator.h"

#include <QDialog>

class GeneratorTableModel;

namespace Ui { class GeneratorDialog; }

class GeneratorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GeneratorDialog(QWidget *parent = nullptr);
    ~GeneratorDialog();

private slots:
    void on_pushButton_openRoot_clicked();
    void on_pushButton_generate_clicked();
    void on_pushButton_openTemplate_clicked();
    void on_pushButton_openProjectLanguage_clicked();

private:
    Ui::GeneratorDialog *ui;

    void translation();

    void reloadTable();
    QString tocDir() const;
    GeneratorTableModel *m_model = nullptr;
    void restoreTocSelection();

    QString writeMarkdown(const QString &absPath,
                          const QString &docId,
                          const QString &tocPath,
                          bool overwrite);
};

#endif // GENERATORDIALOG_H
