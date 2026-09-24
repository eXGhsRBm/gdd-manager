#include "matrixwidget.h"
#include "matrixview.h"

#include "editor.h"
#include "translation.h"

#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QProcess>
#include <QStandardPaths>
#include <QUrl>
#include <QVBoxLayout>

MatrixWidget::MatrixWidget(QWidget *parent)
    : QWidget(parent)
{
    m_view = new MatrixView(this);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_view);

    connect(m_view, &MatrixView::blockDoubleClicked,        this, &MatrixWidget::onBlockDoubleClicked);
    connect(m_view, &MatrixView::blockContextMenuRequested, this, &MatrixWidget::onBlockContextMenu);
    connect(m_view, &MatrixView::emptyContextMenuRequested, this, &MatrixWidget::onEmptyContextMenu);
}

void MatrixWidget::setProject(const ProjectData &data,
                              const QString &projectJsonPath)
{
    m_project = data;
    m_projectJsonPath = projectJsonPath;
    m_view->setProject(data);
}

void MatrixWidget::clearProject()
{
    m_project = {};
    m_projectJsonPath.clear();
    m_view->clearProject();
}

int MatrixWidget::findDocumentIndex(const QString &docId) const
{
    for (auto i = 0; i < m_project.documents.size(); ++i)
    {
        if (m_project.documents[i].id == docId)
        {
            return i;
        }
    }
    return -1;
}

const ProjectDocument *MatrixWidget::findDocument(const QString &docId) const
{
    const auto idx = findDocumentIndex(docId);
    return idx >= 0
               ? &m_project.documents[idx]
               : nullptr;
}

bool MatrixWidget::hasDocument(const QString &docId) const
{
    return findDocumentIndex(docId) >= 0;
}

QString MatrixWidget::documentRelativePath(const ProjectDocument &doc) const
{
    if (doc.files.isEmpty())
    {
        return doc.folder;
    }
    return doc.folder + "/" + doc.files.first();
}

QString MatrixWidget::absolutePath(const QString &docId) const
{
    const auto doc = findDocument(docId);
    if (doc == nullptr)
    {
        return {};
    }

    const auto rel = documentRelativePath(*doc);
    if (m_projectJsonPath.isEmpty())
    {
        return rel;
    }
    return QFileInfo(m_projectJsonPath).absolutePath() + "/" + rel;
}

QString MatrixWidget::defaultEditorProgram(QStringList &args) const
{
    auto &editor    = Editor::instance();
    const auto name = editor.defaultEditor();
    if (name.isEmpty())
    {
        return {};
    }

    const auto ed = editor.get(name);
    if (ed.path.isEmpty())
    {
        return {};
    }

    auto program = ed.path;

    const auto looksLikePath = program.contains(QLatin1Char('/'))
                               || program.contains(QLatin1Char('\\'))
                               || QFileInfo(program).isAbsolute();

    if (looksLikePath == false)
    {
        const auto resolved = QStandardPaths::findExecutable(program);
        if (resolved.isEmpty())
        {
            return {};
        }
        program = resolved;
    }
    else if (QFile::exists(program) == false)
    {
        return {};
    }

    args = ed.args.split(QLatin1Char(' '), Qt::SkipEmptyParts);
    return program;
}

void MatrixWidget::onBlockDoubleClicked(const QString &docId)
{
    openInEditor(docId);
}

void MatrixWidget::onBlockContextMenu(const QString &docId, const QPoint &globalPos)
{
    const auto inProject = hasDocument(docId);

    QMenu menu;

    auto *aOpenEditor = menu.addAction(tr("Open in editor"));
    auto *aOpenFolder = menu.addAction(tr("Open folder"));

    menu.addSeparator();

    auto *aCopyPath = menu.addAction(tr("Copy path"));
    auto *aCopyLink = menu.addAction(tr("Copy link"));
    auto *aCopyId   = menu.addAction(tr("Copy ID"));

    menu.addSeparator();

    auto *aShowToc  = menu.addAction(tr("Show TOC"));

    menu.addSeparator();

    auto *aRename   = menu.addAction(tr("Rename…"));
    auto *aDelete   = menu.addAction(tr("Delete…"));

    aOpenEditor->setEnabled(inProject);
    aOpenFolder->setEnabled(inProject);
    aCopyPath->setEnabled(inProject);
    aCopyLink->setEnabled(inProject);
    aShowToc->setEnabled(inProject);
    aRename->setEnabled(inProject);
    aDelete->setEnabled(inProject);

    auto *chosen = menu.exec(globalPos);
    if (chosen == nullptr)
    {
        return;
    }

    if      (chosen == aOpenEditor) openInEditor(docId);
    else if (chosen == aOpenFolder) openFolder(docId);
    else if (chosen == aCopyPath)   copyPath(docId);
    else if (chosen == aCopyLink)   copyLink(docId);
    else if (chosen == aCopyId)     copyId(docId);
    else if (chosen == aShowToc)    showToc(docId);
    else if (chosen == aRename)     renameDocument(docId);
    else if (chosen == aDelete)     deleteDocument(docId);
}

void MatrixWidget::onEmptyContextMenu(const QPoint &globalPos)
{
    QMenu menu;
    auto *aRefresh = menu.addAction(tr("Refresh"));
    auto *aFit     = menu.addAction(tr("Fit to view"));

    auto *chosen = menu.exec(globalPos);
    if (chosen == nullptr)
    {
        return;
    }

    if (chosen == aRefresh)
    {
        m_view->setProject(m_project);
    }
    else if (chosen == aFit)
    {
        m_view->fitInView(m_view->scene()->itemsBoundingRect(),
                          Qt::KeepAspectRatio);
    }
}

void MatrixWidget::openInEditor(const QString &docId)
{
    const auto path = absolutePath(docId);
    if (path.isEmpty() || QFile::exists(path) == false)
    {
        QMessageBox::warning(this,
                             tr("File not found"),
                             path);
        return;
    }

    QStringList args;
    const auto program = defaultEditorProgram(args);
    if (program.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("No editor configured"),
                             tr("Set default editor in Settings"));
        return;
    }

    for (auto &a : args)
    {
        a.replace(QStringLiteral("{file}"), path);
        a.replace(QStringLiteral("{line}"), QStringLiteral("1"));
    }

    if (QProcess::startDetached(program, args) == false)
    {
        QMessageBox::warning(this,
                             tr("Failed to launch editor"),
                             program);
    }
}

void MatrixWidget::openFolder(const QString &docId)
{
    const auto path = absolutePath(docId);
    if (path.isEmpty())
    {
        return;
    }

    const auto folder = QFileInfo(path).absolutePath();
    QDesktopServices::openUrl(QUrl::fromLocalFile(folder));
}

void MatrixWidget::copyPath(const QString &docId)
{
    const auto path = absolutePath(docId);
    if (path.isEmpty() == false)
    {
        QApplication::clipboard()->setText(path);
    }
}

void MatrixWidget::copyLink(const QString &docId)
{
    const auto doc = findDocument(docId);
    if (doc == nullptr)
    {
        return;
    }

    const auto rel  = documentRelativePath(*doc);
    const auto name = TRANSLATE_DOC("document." + docId);

    const auto md = QStringLiteral("[%1](%2)").arg(name, rel);
    QApplication::clipboard()->setText(md);
}

void MatrixWidget::copyId(const QString &docId)
{
    QApplication::clipboard()->setText(docId);
}

void MatrixWidget::showToc(const QString &docId)
{
    const auto doc = findDocument(docId);
    if (doc == nullptr)
    {
        return;
    }

    const auto tocRel = doc->tocPath;
    if (tocRel.isEmpty())
    {
        QMessageBox::information(this,
                                 tr("No TOC"),
                                 tr("Document has no TOC"));
        return;
    }

    const auto tocAbs = QFileInfo(tocRel).isAbsolute()
                            ? tocRel
                            : QFileInfo(m_projectJsonPath).absolutePath() + "/" + tocRel;

    if (QFile::exists(tocAbs) == false)
    {
        QMessageBox::warning(this,
                             tr("File not found"),
                             tocAbs);
        return;
    }

    QStringList args;
    const auto program = defaultEditorProgram(args);
    if (program.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("No editor configured"),
                             tr("Set default editor in Settings"));
        return;
    }

    for (auto &a : args)
    {
        a.replace(QStringLiteral("{file}"), tocAbs);
        a.replace(QStringLiteral("{line}"), QStringLiteral("1"));
    }

    QProcess::startDetached(program, args);
}

void MatrixWidget::renameDocument(const QString &docId)
{
    const auto idx = findDocumentIndex(docId);
    if (idx < 0)
    {
        return;
    }

    const auto doc = m_project.documents[idx];

    bool ok = false;
    const auto oldName = doc.folder;
    const auto newName = QInputDialog::getText(this,
                                               tr("Rename document"),
                                               tr("New name:"),
                                               QLineEdit::Normal, oldName, &ok);

    if (ok == false || newName.trimmed().isEmpty())
    {
        return;
    }

    const auto cleanName = newName.trimmed();
    if (cleanName == oldName)
    {
        return;
    }

    const auto newFolder = cleanName;
    const auto oldFile   = oldName + ".md";
    const auto newFile   = cleanName + ".md";

    const auto projectDir   = QFileInfo(m_projectJsonPath).absolutePath();
    const auto oldFolderAbs = projectDir + "/" + oldName;
    const auto newFolderAbs = projectDir + "/" + newFolder;

    if (oldFolderAbs != newFolderAbs)
    {
        if (QDir().rename(oldFolderAbs, newFolderAbs) == false)
        {
            QMessageBox::warning(this,
                                 tr("Error"),
                                 tr("Failed to rename folder"));
            return;
        }
    }

    const auto oldFileAbs = newFolderAbs + "/" + oldFile;
    const auto newFileAbs = newFolderAbs + "/" + newFile;

    if (oldFileAbs != newFileAbs && QFile::exists(oldFileAbs))
    {
        if (QFile::rename(oldFileAbs, newFileAbs) == false)
        {
            QDir().rename(newFolderAbs, oldFolderAbs);
            QMessageBox::warning(this,
                                 tr("Error"),
                                 tr("Failed to rename file"));
            return;
        }
    }

    ProjectDocument updated = doc;
    updated.folder = newFolder;
    updated.files.clear();
    updated.files << newFile;

    m_project.documents[idx] = updated;

    if (Project::save(m_projectJsonPath, m_project) == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Failed to save project.json"));
        return;
    }

    m_view->setProject(m_project);
    emit projectModified();
}

void MatrixWidget::deleteDocument(const QString &docId)
{
    const auto idx = findDocumentIndex(docId);
    if (idx < 0)
    {
        return;
    }

    const auto reply = QMessageBox::question(this,
                                             tr("Delete document"),
                                             tr("Delete document \"%1\" with folder and files?").arg(docId),
                                             QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) return;

    const auto path   = absolutePath(docId);
    const auto folder = QFileInfo(path).absolutePath();

    if (QFileInfo(folder).isDir())
    {
        if (QDir(folder).removeRecursively() == false)
        {
            QMessageBox::warning(this,
                                 tr("Error"),
                                 tr("Failed to delete folder"));
            return;
        }
    }

    m_project.documents.removeAt(idx);

    if (Project::save(m_projectJsonPath, m_project) == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Failed to save project.json"));
        return;
    }

    m_view->setProject(m_project);
    emit projectModified();
}