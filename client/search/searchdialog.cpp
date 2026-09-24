#include "searchdialog.h"
#include "ui_searchdialog.h"

#include "search.h"
#include "foundtreemodel.h"
#include "editor.h"

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHeaderView>
#include <QMessageBox>
#include <QProcess>
#include <QStandardPaths>
#include <QUrl>

SearchDialog::SearchDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SearchDialog)
{
    ui->setupUi(this);

    m_model = new FoundTreeModel(this);
    ui->treeView->setModel(m_model);

    setupTree();

    connect(ui->treeView, &QTreeView::doubleClicked, this, &SearchDialog::onResultDoubleClicked);
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &SearchDialog::on_pushButton_search_clicked);

    translation();
}

SearchDialog::~SearchDialog()
{
    delete ui;
}

void SearchDialog::translation()
{
    setWindowTitle(tr("Search"));

    ui->lineEdit->setPlaceholderText(tr("Phrase for search"));
    ui->pushButton_search->setText(tr("Search"));

    ui->checkBox_folderName->setText(tr("Search in folder names"));
    ui->checkBox_fileName->setText(tr("Search in file names"));
    ui->checkBox_content->setText(tr("Search in documents"));

    if (m_model) {
        m_model->setHorizontalHeaderLabels({
            tr("File"),
            tr("Line"),
            tr("Fragment")
        });
    }
}

void SearchDialog::setupTree()
{
    ui->treeView->setHeaderHidden(false);
    ui->treeView->setRootIsDecorated(true);
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeView->setUniformRowHeights(true);

    auto *header = ui->treeView->header();
    header->setSectionResizeMode(FoundTreeModel::ColFile,     QHeaderView::ResizeToContents);
    header->setSectionResizeMode(FoundTreeModel::ColLine,     QHeaderView::ResizeToContents);
    header->setSectionResizeMode(FoundTreeModel::ColFragment, QHeaderView::Stretch);
}

void SearchDialog::on_pushButton_search_clicked()
{
    const auto phrase = ui->lineEdit->text().trimmed();
    if (phrase.isEmpty())
    {
        m_model->clearResults();
        return;
    }

    if (m_projectDir.isEmpty() || QDir(m_projectDir).exists() == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Project folder is not available"));
        return;
    }

    SearchOptions opts;
    opts.folders = ui->checkBox_folderName->isChecked();
    opts.files   = ui->checkBox_fileName->isChecked();
    opts.content = ui->checkBox_content->isChecked();

    if (opts.folders == false && opts.files == false && opts.content == false)
    {
        m_model->clearResults();
        return;
    }

    const auto matches = Search::find(m_projectDir, phrase, opts);
    m_model->setResults(matches);
    ui->treeView->expandAll();
}

void SearchDialog::onResultDoubleClicked(const QModelIndex &idx)
{
    if (idx.isValid() == false)
    {
        return;
    }

    const auto path = m_model->pathAt(idx);
    const auto line = m_model->lineAt(idx);

    if (path.isEmpty())
    {
        return;
    }

    const QFileInfo fi(path);
    if (fi.isDir())
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        return;
    }

    if (fi.isFile() == false)
    {
        QMessageBox::information(this,
                                 tr("File not found"),
                                 path);
        return;
    }

    openInEditor(path, line);
}

void SearchDialog::openInEditor(const QString &path,
                                int line)
{
    if (path.isEmpty() || QFile::exists(path) == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("File not found:\n%1").arg(path));
        return;
    }

    auto &editor = Editor::instance();
    const auto defaultName = editor.defaultEditor();

    if (defaultName.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("No editor configured. "
                                "Set it in Settings → External editors."));
        return;
    }

    const auto ed = editor.get(defaultName);
    if (ed.path.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Editor \"%1\" has no path.").arg(defaultName));
        return;
    }

    auto program = ed.path;

    const auto looksLikePath = program.contains(QLatin1Char('/'))
                               || program.contains(QLatin1Char('\\'))
                               || QFileInfo(program).isAbsolute();

    if (looksLikePath)
    {
        if (QFile::exists(program) == false)
        {
            QMessageBox::warning(this,
                                 tr("Error"),
                                 tr("Editor executable not found:\n%1").arg(program));
            return;
        }
    }
    else
    {
        const auto resolved = QStandardPaths::findExecutable(program);
        if (resolved.isEmpty())
        {
            QMessageBox::warning(this,
                                 tr("Error"),
                                 tr("Editor \"%1\" not found in PATH.").arg(program));
            return;
        }
        program = resolved;
    }

    auto args = ed.args.split(QLatin1Char(' '), Qt::SkipEmptyParts);
    for (auto &a : args)
    {
        a.replace(QStringLiteral("{file}"), path);
        a.replace(QStringLiteral("{line}"), QString::number(line));
    }

    if (QProcess::startDetached(program, args) == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Failed to launch editor:\n%1").arg(program));
    }
}