#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "settings.h"
#include "translator.h"
#include "theme.h"
#include "editor.h"

#include <QDialog>
#include <QLineEdit>

class EditorsTableModel;

namespace Ui { class SettingsDialog; }

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

signals:
    void themeChanged();

private slots:
    void on_pushButton_applyLanguages_clicked();

    void on_pushButton_addEditor_clicked();
    void on_pushButton_applyEditor_clicked();
    void on_pushButton_checkEditor_clicked();

    void on_pushButton_programTextColor_clicked();
    void on_pushButton_programBgColor_clicked();

    void on_pushButton_matrixBgColor_clicked();

    void on_pushButton_blockTextColor_clicked();
    void on_pushButton_blockBgColor_clicked();

    void on_comboBox_themes_currentIndexChanged(int index);

    void on_pushButton_resetThemes_clicked();
    void on_pushButton_applyThemes_clicked();

    void onEditorsContextMenu(const QPoint &pos);

private:
    Ui::SettingsDialog *ui;

    EditorsTableModel *m_editorsModel = nullptr;

    void translation();
    void populateLanguages();
    void populateThemes();
    void populateEditors();
    void refreshDefaultEditorCombo();

    ThemeData changedTheme;
    ThemeData currentTheme;

    void loadTheme();
    void pickColor(QLineEdit *lineEdit,
                   const QColor &current);
};

#endif // DIALOG_SETTINGS_H
