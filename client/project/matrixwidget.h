#ifndef MATRIXWIDGET_H
#define MATRIXWIDGET_H

#include <QWidget>
#include "project.h"

class MatrixView;

class MatrixWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MatrixWidget(QWidget *parent = nullptr);

    void setProject(const ProjectData &data,
                    const QString &projectJsonPath);
    void clearProject();

    MatrixView *view() const
    { return m_view; }

signals:
    void projectModified();
    void blockActivated(const QString &docId);

private slots:
    void onBlockDoubleClicked(const QString &docId);
    void onBlockContextMenu(const QString &docId,
                            const QPoint &globalPos);
    void onEmptyContextMenu(const QPoint &globalPos);

private:
    void openInEditor(const QString &docId);
    void openFolder(const QString &docId);
    void copyPath(const QString &docId);
    void copyLink(const QString &docId);
    void copyId(const QString &docId);
    void showToc(const QString &docId);
    void renameDocument(const QString &docId);
    void deleteDocument(const QString &docId);

    QString absolutePath(const QString &docId) const;
    QString defaultEditorProgram(QStringList &args) const;

    MatrixView *m_view = nullptr;
    ProjectData m_project;
    QString m_projectJsonPath;

    int findDocumentIndex(const QString &docId) const;
    const ProjectDocument *findDocument(const QString &docId) const;
    QString documentRelativePath(const ProjectDocument &doc) const;
    bool hasDocument(const QString &docId) const;
};

#endif // MATRIXWIDGET_H