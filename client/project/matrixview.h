#ifndef MATRIXVIEW_H
#define MATRIXVIEW_H

#include <QGraphicsView>
#include "project.h"

class MatrixView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit MatrixView(QWidget *parent = nullptr);

    void setProject(const ProjectData &data);
    void clearProject();

signals:
    void blockClicked(const QString &docId);
    void blockDoubleClicked(const QString &docId);
    void blockContextMenuRequested(const QString &docId,
                                   const QPoint &globalPos);
    void emptyContextMenuRequested(const QPoint &globalPos);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QString docIdAt(const QPoint &viewPos) const;

    QGraphicsScene *m_scene = nullptr;
    ProjectData m_project;
};

#endif // MATRIXVIEW_H