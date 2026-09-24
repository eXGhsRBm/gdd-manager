#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "settings.h"
#include "editorstablemodel.h"

#include <QDialog>
#include <QDirIterator>
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    currentTheme = changedTheme = Theme::instance().current();

    ui->tableView_editors->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_editors, &QTableView::customContextMenuRequested, this, &SettingsDialog::onEditorsContextMenu);

    populateLanguages();
    populateEditors();
    populateThemes();

    translation();
}

void SettingsDialog::translation()
{
    setWindowTitle(tr("Settings"));

    ui->tabWidget->setTabText(0, tr("Languages"));
    ui->tabWidget->setTabText(1, tr("External editors"));
    ui->tabWidget->setTabText(2, tr("Themes"));

    ui->pushButton_applyLanguages->setText(tr("Apply"));

    ui->pushButton_addEditor->setText(tr("Add editor"));
    ui->pushButton_applyEditor->setText(tr("Apply"));
    ui->pushButton_checkEditor->setText(tr("Check editor"));

    ui->label_activeTheme->setText(tr("Active theme"));

    ui->groupBox_programTheme->setTitle(tr("Program"));
    ui->label_programText->setText(tr("Text height"));
    ui->label_programTextColor->setText(tr("Text color"));
    ui->pushButton_programTextColor->setText(tr("Choose"));
    ui->label_programBgColor->setText(tr("Background color"));
    ui->pushButton_programBgColor->setText(tr("Choose"));

    ui->groupBox_matrixTheme->setTitle(tr("Matrix"));
    ui->label_matrixBgColor->setText(tr("Background color"));
    ui->pushButton_matrixBgColor->setText(tr("Choose"));

    ui->groupBox_blockTheme->setTitle(tr("Blocks"));
    ui->label_blockText->setText(tr("Text height"));
    ui->label_blockTextColor->setText(tr("Text color"));
    ui->pushButton_blockTextColor->setText(tr("Choose"));
    ui->label_blockBgColor->setText(tr("Background color"));
    ui->pushButton_blockBgColor->setText(tr("Choose"));

    ui->pushButton_resetThemes->setText(tr("Reset"));
    ui->pushButton_applyThemes->setText(tr("Apply"));

    if (m_editorsModel)
    {
        m_editorsModel->setHorizontalHeaderLabels({
            tr("Name"),
            tr("Path"),
            tr("Arguments")
        });
    }
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
