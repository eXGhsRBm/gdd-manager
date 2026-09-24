#ifndef DOCUMENTBLOCKITEM_H
#define DOCUMENTBLOCKITEM_H

#include <QGraphicsRectItem>

class DocumentBlockItem : public QGraphicsRectItem
{
public:
    DocumentBlockItem(const QString &docId,
                      const QColor &categoryColor,
                      const QColor &stageColor,
                      const QColor &bgColor,
                      QGraphicsItem *parent = nullptr);

    QString documentId() const
    { return m_docId; }

    void setExistsInProject(bool exists);

    bool existsInProject() const
    { return m_exists; }

protected:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

private:
    QString m_docId;
    QColor  m_categoryColor;
    QColor  m_stageColor;
    QColor  m_bgColor;
    bool    m_exists = false;
};

#endif // DOCUMENTBLOCKITEM_H