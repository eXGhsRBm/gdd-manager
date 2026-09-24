#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "settings.h"

#include <QDirIterator>
#include <QMessageBox>

void SettingsDialog::populateLanguages()
{
    ui->comboBox_languages->clear();

    const auto langDir = APP_DIR + "/languages/ui";
    QDirIterator it(langDir, {QStringLiteral("*.json")}, QDir::Files);
    while (it.hasNext())
    {
        const QFileInfo fi(it.next());
        const auto code = fi.baseName();
        ui->comboBox_languages->addItem(code, code);
    }

    const auto current = QFileInfo(LANGUAGE_FILE_PATH).baseName();
    const auto idx     = ui->comboBox_languages->findData(current);
    if (idx >= 0)
    {
        ui->comboBox_languages->setCurrentIndex(idx);
    }
}

void SettingsDialog::on_pushButton_applyLanguages_clicked()
{
    const auto code = ui->comboBox_languages->currentData().toString();
    if (code.isEmpty())
    {
        return;
    }
    const auto newPath = APP_DIR + "/languages/ui/" + code + ".json";
    if (QFile::exists(newPath) == false)
    {
        return;
    }
    if (newPath != LANGUAGE_FILE_PATH)
    {
        LANGUAGE_FILE_PATH = newPath;
        Settings::instance().save();

        QMessageBox::information(this,
                                 tr("Language changed"),
                                 tr("Language changed. Please restart the application."));

        accept();
    }
}