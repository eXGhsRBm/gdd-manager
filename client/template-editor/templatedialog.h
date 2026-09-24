#ifndef TEMPLATEDIALOG_H
#define TEMPLATEDIALOG_H

#include "template.h"
#include "translator.h"

#include <QDialog>

class CategoriesTableModel;
class StagesTableModel;
class DocsTableModel;

namespace Ui { class TemplateDialog; }

class TemplateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TemplateDialog(QWidget *parent = nullptr);
    ~TemplateDialog();

private slots:
    void on_pushButton_addCategory_clicked();
    void on_pushButton_saveCategories_clicked();

    void on_pushButton_addStage_clicked();
    void on_pushButton_saveStages_clicked();

    void on_pushButton_addDoc_clicked();
    void on_pushButton_saveDocs_clicked();

    void onTableCategoriesContextMenu(const QPoint &pos);
    void onTableStagesContextMenu(const QPoint &pos);
    void onTableDocsContextMenu(const QPoint &pos);

    void on_pushButton_addTemplate_clicked();
    void on_comboBox_templates_currentIndexChanged(int index);

    void on_pushButton_resetCategoryChanges_clicked();
    void on_pushButton_resetStageChanges_clicked();
    void on_pushButton_resetDocChanges_clicked();

private:
    Ui::TemplateDialog *ui;

    CategoriesTableModel *m_catModel  = nullptr;
    StagesTableModel     *m_stageModel = nullptr;
    DocsTableModel       *m_docModel   = nullptr;

    TemplateData m_savedData;
    bool         m_isSample = false;

    void translation();
    void setupTables();
    void setupDelegates();

    void saveTemplate();

    bool isSample(const QString &fileName) const;
    void updateActionsState();
    void applyData(const TemplateData &data);

    bool confirmCategoryRemoval(int row);
    bool confirmStageRemoval(int row);

    void populateTemplates();
    void loadTemplateFrom(const QString &path);
    QString templatesDir() const;
    QString selectedTemplatePath() const;
};

#endif