#include "scannerdialog.h"
#include "ui_scannerdialog.h"

#include "scanner.h"
#include "issuestreemodel.h"
#include "editor.h"

#include <QDesktopServices>
#include <QFileInfo>
#include <QHeaderView>
#include <QMessageBox>
#include <QProcess>
#include <QStandardPaths>
#include <QUrl>

ScannerDialog::ScannerDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ScannerDialog)
{
    ui->setupUi(this);

    m_model = new IssuesTreeModel(this);
    ui->treeView->setModel(m_model);

    setupTree();

    connect(ui->treeView, &QTreeView::doubleClicked, this, &ScannerDialog::onIssueDoubleClicked);

    translation();
}

ScannerDialog::~ScannerDialog()
{
    delete ui;
}

void ScannerDialog::translation()
{
    setWindowTitle(tr("Scanner"));
    ui->pushButton_scan->setText(tr("Scan"));

    m_model->setHorizontalHeaderLabels({
        tr("File"),
        tr("Error"),
        tr("Details"),
        tr("Line")
    });
}

void ScannerDialog::setupTree()
{
    ui->treeView->setHeaderHidden(false);
    ui->treeView->setRootIsDecorated(true);
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeView->setUniformRowHeights(true);

    auto *header = ui->treeView->header();
    header->setSectionResizeMode(IssuesTreeModel::ColFile,    QHeaderView::ResizeToContents);
    header->setSectionResizeMode(IssuesTreeModel::ColError,   QHeaderView::ResizeToContents);
    header->setSectionResizeMode(IssuesTreeModel::ColDetails, QHeaderView::Stretch);
    header->setSectionResizeMode(IssuesTreeModel::ColLine,    QHeaderView::ResizeToContents);
}

void ScannerDialog::on_pushButton_scan_clicked()
{
    if (m_projectDir.isEmpty() || m_projectJsonPath.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Project is not open"));
        return;
    }

    const auto issues = Scanner::scan(m_projectDir, m_projectJsonPath);
    m_model->setIssues(issues);

    if (issues.isEmpty())
    {
        QMessageBox::information(this,
                                 tr("Scanner"),
                                 tr("No issues found"));
        return;
    }
    ui->treeView->expandAll();
}

void ScannerDialog::onIssueDoubleClicked(const QModelIndex &idx)
{
    if (idx.isValid() == false)
    {
        return;
    }

    const auto path = m_model->pathAt(idx);
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

    const auto line = m_model->lineAt(idx);
    openInEditor(path, line);
}

void ScannerDialog::openInEditor(const QString &path,
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