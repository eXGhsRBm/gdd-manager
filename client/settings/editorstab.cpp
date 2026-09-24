#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "editor.h"
#include "editorstablemodel.h"

#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>

void SettingsDialog::onEditorsContextMenu(const QPoint &pos)
{
    if (m_editorsModel == nullptr)
    {
        return;
    }

    const auto idx = ui->tableView_editors->indexAt(pos);
    if (idx.isValid() == false)
    {
        return;
    }

    QMenu menu;
    auto actDelete = menu.addAction(tr("Delete row"));
    auto chosen    = menu.exec( ui->tableView_editors->viewport()->mapToGlobal(pos));
    if (chosen == actDelete)
    {
        m_editorsModel->removeRow(idx.row());
    }
}

void SettingsDialog::populateEditors()
{
    if (m_editorsModel == nullptr)
    {
        m_editorsModel = new EditorsTableModel(this);
    }
    ui->tableView_editors->setModel(m_editorsModel);

    auto *header = ui->tableView_editors->horizontalHeader();
    header->setSectionsMovable(false);
    header->setSectionResizeMode(EditorsTableModel::ColName, QHeaderView::Interactive);
    header->setSectionResizeMode(EditorsTableModel::ColPath, QHeaderView::Interactive);
    header->setSectionResizeMode(EditorsTableModel::ColArgs, QHeaderView::Stretch);
    ui->tableView_editors->setColumnWidth(EditorsTableModel::ColName, 150);
    ui->tableView_editors->setColumnWidth(EditorsTableModel::ColPath, 300);
    ui->tableView_editors->verticalHeader()->setVisible(false);

    m_editorsModel->loadFromEditors();
    refreshDefaultEditorCombo();
}

void SettingsDialog::refreshDefaultEditorCombo()
{
    const auto current = Editor::instance().defaultEditor();

    QSignalBlocker blocker(ui->comboBox_defaultEditor);
    ui->comboBox_defaultEditor->clear();

    for (const auto &name : Editor::instance().names())
    {
        ui->comboBox_defaultEditor->addItem(name, name);
    }

    const auto idx = ui->comboBox_defaultEditor->findData(current);
    if (idx >= 0)
    {
        ui->comboBox_defaultEditor->setCurrentIndex(idx);
    }
    else if (ui->comboBox_defaultEditor->count() > 0)
    {
        ui->comboBox_defaultEditor->setCurrentIndex(0);
    }
}

void SettingsDialog::on_pushButton_addEditor_clicked()
{
    if (m_editorsModel == nullptr)
    {
        return;
    }

    const auto file = QFileDialog::getOpenFileName(this,
                                       tr("Select editor executable"),
                                       QDir::homePath(),
                                       QStringLiteral("Executables (*.exe *.sh *.app);;All files (*)"));

    if (file.isEmpty())
    {
        return;
    }

    const QFileInfo fi(file);
    auto name = fi.completeBaseName();
    if (name.isEmpty())
    {
        name = tr("New editor");
    }

    EditorData e;
    e.name = name;
    e.path = QDir::toNativeSeparators(file);
    e.args = QStringLiteral("{file}:{line}");

    m_editorsModel->addRow(e);

    const auto last = m_editorsModel->rowCount() - 1;
    if (last >= 0)
    {
        ui->tableView_editors->selectRow(last);
    }
}

void SettingsDialog::on_pushButton_applyEditor_clicked()
{
    if (m_editorsModel == nullptr)
    {
        return;
    }

    QSet<QString> seen;
    QStringList duplicates;
    for (auto r = 0; r < m_editorsModel->rowCount(); ++r)
    {
        const auto name = m_editorsModel->data(m_editorsModel->index(r, EditorsTableModel::ColName), Qt::DisplayRole).toString();
        if (name.isEmpty())
        {
            continue;
        }

        if (seen.contains(name))
        {
            duplicates << name;
        }

        seen.insert(name);
    }

    if (duplicates.isEmpty() == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Duplicate editor names: %1").arg(duplicates.join(", ")));
        return;
    }
    const auto defaultName = ui->comboBox_defaultEditor->currentData().toString();

    m_editorsModel->saveToEditors();

    if (defaultName.isEmpty() == false)
    {
        Editor::instance().setDefault(defaultName);
    }

    if (Editor::instance().save() == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Failed to save editors"));
        return;
    }

    refreshDefaultEditorCombo();

    QMessageBox::information(this,
                             tr("Done"),
                             tr("Editors saved"));
}

void SettingsDialog::on_pushButton_checkEditor_clicked()
{
    const auto name = ui->comboBox_defaultEditor->currentData().toString();
    if (name.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Check editor"),
                             tr("No editor selected"));
        return;
    }

    const auto ed = Editor::instance().get(name);
    if (ed.path.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Check editor"),
                             tr("Editor \"%1\" has no path").arg(name));
        return;
    }

    QString resolved;
    bool found = false;

    const bool looksLikePath = ed.path.contains(QLatin1Char('/'))
                               || ed.path.contains(QLatin1Char('\\'))
                               || QFileInfo(ed.path).isAbsolute();

    if (looksLikePath)
    {
        found = QFile::exists(ed.path);
        resolved = ed.path;
    }
    else
    {
        resolved = QStandardPaths::findExecutable(ed.path);
        found = !resolved.isEmpty();
    }

    if (found == false)
    {
        QMessageBox::warning(this,
                             tr("Check editor"),
                             tr("Executable not found:\n%1").arg(ed.path));
        return;
    }

    QStringList warnings;

    if (ed.args.contains(QStringLiteral("{file}")) == false)
    {
        warnings << tr("Arguments do not contain {file} — "
                       "the editor will not know which file to open");
    }

    if (ed.args.contains(QStringLiteral("{line}")) == false)
    {
        warnings << tr("Arguments do not contain {line} — "
                       "the editor will open the file at the beginning");
    }

    auto msg = tr("Editor: %1").arg(name) + "\n"
                  + tr("Path: %1").arg(resolved) + "\n"
                  + tr("Arguments: %1").arg(ed.args);

    if (warnings.isEmpty())
    {
        QMessageBox::information(this,
                                 tr("Check editor"),
                                 msg + "\n\n" + tr("OK — editor is available"));
    }
    else
    {
        QMessageBox::warning(this,
                             tr("Check editor"),
                             msg + "\n\n" + warnings.join("\n"));
    }
}