#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "loader.h"

#include <QColorDialog>
#include <QMessageBox>

void SettingsDialog::populateThemes()
{
    ui->comboBox_themes->clear();

    for (const auto &name : Theme::instance().names())
    {
        ui->comboBox_themes->addItem(name, name);
    }

    const auto idx = ui->comboBox_themes->findText(Theme::instance().currentName());
    if (idx >= 0)
    {
        ui->comboBox_themes->setCurrentIndex(idx);
    }
}

void SettingsDialog::loadTheme()
{
    ui->spinBox_programTextSize->setValue(changedTheme.programTextSize);
    ui->spinBox_blockTextSize->setValue(changedTheme.blockTextSize);

    pickColor(ui->lineEdit_programTextColor, changedTheme.programTextColor);
    pickColor(ui->lineEdit_programBgColor,   changedTheme.programBgColor);
    pickColor(ui->lineEdit_matrixBgColor,    changedTheme.matrixBgColor);
    pickColor(ui->lineEdit_blockTextColor,   changedTheme.blockTextColor);
    pickColor(ui->lineEdit_blockBgColor,     changedTheme.blockBgColor);
}

void SettingsDialog::pickColor(QLineEdit *lineEdit,
                               const QColor &current)
{
    lineEdit->setStyleSheet(QString(
                                "QLineEdit { "
                                "  background-color: %1; "
                                "  border: 1px solid #888; "
                                "  border-radius: 3px; "
                                "}"
                                ).arg(current.name()));
}

void SettingsDialog::on_comboBox_themes_currentIndexChanged(int)
{
    const auto id = ui->comboBox_themes->currentData().toString();
    if (id.isEmpty())
    {
        return;
    }
    currentTheme = changedTheme = Theme::instance().get(id);
    loadTheme();
}

void SettingsDialog::on_pushButton_programTextColor_clicked()
{
    changedTheme.programTextColor = QColorDialog::getColor(changedTheme.programTextColor,
                                                           this,
                                                           tr("Select color"));

    pickColor(ui->lineEdit_programTextColor, changedTheme.programTextColor);
}

void SettingsDialog::on_pushButton_programBgColor_clicked()
{
    changedTheme.programBgColor = QColorDialog::getColor(changedTheme.programBgColor,
                                                         this,
                                                         tr("Select color"));

    pickColor(ui->lineEdit_programBgColor, changedTheme.programBgColor);
}

void SettingsDialog::on_pushButton_matrixBgColor_clicked()
{
    changedTheme.matrixBgColor = QColorDialog::getColor(changedTheme.matrixBgColor,
                                                        this,
                                                        tr("Select color"));

    pickColor(ui->lineEdit_matrixBgColor, changedTheme.matrixBgColor);
}

void SettingsDialog::on_pushButton_blockTextColor_clicked()
{
    changedTheme.blockTextColor = QColorDialog::getColor(changedTheme.blockTextColor,
                                                         this,
                                                         tr("Select color"));

    pickColor(ui->lineEdit_blockTextColor, changedTheme.blockTextColor);
}

void SettingsDialog::on_pushButton_blockBgColor_clicked()
{
    changedTheme.blockBgColor = QColorDialog::getColor(changedTheme.blockBgColor,
                                                       this,
                                                       tr("Select color"));

    pickColor(ui->lineEdit_blockBgColor, changedTheme.blockBgColor);
}

void SettingsDialog::on_pushButton_resetThemes_clicked()
{
    const auto name = changedTheme.name;
    changedTheme = currentTheme;
    changedTheme.name = name;
    loadTheme();
}

void SettingsDialog::on_pushButton_applyThemes_clicked()
{
    if (changedTheme.name.isEmpty())
    {
        return;
    }

    changedTheme.programBorderColor = changedTheme.programTextColor;

    changedTheme.programTextSize = ui->spinBox_programTextSize->value();
    changedTheme.blockTextSize   = ui->spinBox_blockTextSize->value();

    Theme::instance().set(changedTheme.name, changedTheme );

    Theme::instance().setCurrent(changedTheme.name);

    if (Theme::instance().save())
    {
        currentTheme = changedTheme;
        emit themeChanged();
        QMessageBox::information(this,
                                 tr("Done"),
                                 tr("Theme applied"));
    }
    else
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Failed to save settings"));
    }
}

