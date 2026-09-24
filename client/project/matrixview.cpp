#include "matrixview.h"

#include "documentblockitem.h"
#include "translation.h"
#include "theme.h"

#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QWheelEvent>

namespace {

constexpr int BLOCK_H = 80;

constexpr int BLOCK_GAP_H = 20;
constexpr int BLOCK_GAP_V = 20;

constexpr int CAT_W   = 180;
constexpr int STAGE_W = 200;
constexpr int DOC_W   = 220;

constexpr int LEFT_PAD = CAT_W + BLOCK_GAP_H;

const QColor DEFAULT_CAT_BORDER   = QColor("#6c8ebf");
const QColor DEFAULT_STAGE_BORDER = QColor("#000000");

} // namespace

MatrixView::MatrixView(QWidget *parent)
    : QGraphicsView(parent)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing, true);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setContextMenuPolicy(Qt::DefaultContextMenu);
}

void MatrixView::clearProject()
{
    m_project = {};
    m_scene->clear();
}

void MatrixView::setProject(const ProjectData &data)
{
    m_project = data;
    m_scene->clear();

    const auto &cats   = data.categories;
    const auto &stages = data.stages;
    if (cats.isEmpty() || stages.isEmpty())
    {
        return;
    }

    const auto theme = Theme::instance().current();

    const auto blockBg   = theme.blockBgColor.isValid()
                             ? theme.blockBgColor
                             : QColor("#ffffff");
    const auto blockText = theme.blockTextColor.isValid()
                               ? theme.blockTextColor
                               : QColor("#000000");
    const auto blockSize = theme.blockTextSize > 0
                               ? theme.blockTextSize
                               : 11;

    QFont blockFont;
    blockFont.setPointSize(blockSize);

    QHash<QString, QColor> catBorderById;
    for (const auto &cat : cats)
    {
        catBorderById.insert(cat.id, cat.border.isValid() ? cat.border : DEFAULT_CAT_BORDER);
    }

    QHash<QString, QColor> stageBorderById;
    for (const auto &st : stages)
    {
        stageBorderById.insert(st.id, st.border.isValid() ? st.border : DEFAULT_STAGE_BORDER);
    }

    QVector<int> colW(stages.size(), STAGE_W);
    for (auto c = 0; c < stages.size(); ++c)
    {
        int maxDocs = 0;
        for (auto r = 0; r < cats.size(); ++r)
        {
            int n = 0;
            for (const auto &d : data.documents)
            {
                if (d.category == cats[r].id && d.stage == stages[c].id)
                {
                    ++n;
                }
            }
            maxDocs = qMax(maxDocs, n);
        }
        const auto docsW = maxDocs > 0
                              ? maxDocs * DOC_W + (maxDocs - 1) * BLOCK_GAP_H
                              : 0;
        colW[c] = qMax(STAGE_W, docsW);
    }

    QVector<int> colX(stages.size());
    auto x = LEFT_PAD;
    for (auto c = 0; c < stages.size(); ++c)
    {
        colX[c] = x;
        x += colW[c] + BLOCK_GAP_H;
    }

    for (auto c = 0; c < stages.size(); ++c)
    {
        const auto border = stageBorderById.value(stages[c].id, DEFAULT_STAGE_BORDER);
        const auto label  = TRANSLATE_DOC("stage." + stages[c].id);

        m_scene->addRect(colX[c], 0, colW[c], BLOCK_H, QPen(border, 3), QBrush(blockBg));

        auto *txt = m_scene->addSimpleText(label, blockFont);
        txt->setBrush(blockText);
        const auto tr = txt->boundingRect();
        txt->setPos(colX[c] + (colW[c] - tr.width()) / 2, (BLOCK_H - tr.height()) / 2);
    }

    auto y = BLOCK_H + BLOCK_GAP_V;

    for (auto r = 0; r < cats.size(); ++r)
    {
        const auto catBorder = catBorderById.value(cats[r].id, DEFAULT_CAT_BORDER);
        const auto catLabel  = TRANSLATE_DOC("category." + cats[r].id);

        m_scene->addRect(0, y, CAT_W, BLOCK_H, QPen(catBorder, 3), QBrush(blockBg));

        auto *catItem = m_scene->addSimpleText(catLabel, blockFont);
        catItem->setBrush(blockText);
        const auto cr = catItem->boundingRect();
        catItem->setPos((CAT_W - cr.width()) / 2, y + (BLOCK_H - cr.height()) / 2);

        for (auto c = 0; c < stages.size(); ++c)
        {
            const auto stageBorder = stageBorderById.value(stages[c].id, DEFAULT_STAGE_BORDER);
            auto bx = colX[c];

            for (const auto &d : data.documents)
            {
                if (d.category != cats[r].id || d.stage != stages[c].id)
                {
                    continue;
                }

                auto *block = new DocumentBlockItem(d.id, catBorder, stageBorder, blockBg);
                block->setRect(bx, y, DOC_W, BLOCK_H);
                m_scene->addItem(block);

                const auto docLabel = TRANSLATE_DOC("document." + d.id);
                auto *txt = m_scene->addSimpleText(docLabel, blockFont);
                txt->setBrush(blockText);
                const auto tr = txt->boundingRect();
                txt->setPos(bx + (DOC_W - tr.width()) / 2, y + (BLOCK_H - tr.height()) / 2);

                bx += DOC_W + BLOCK_GAP_H;
            }
        }

        y += BLOCK_H + BLOCK_GAP_V;
    }

    m_scene->setSceneRect(m_scene->itemsBoundingRect().adjusted(-20, -20, 20, 20));
}

QString MatrixView::docIdAt(const QPoint &viewPos) const
{
    const auto scenePos = mapToScene(viewPos);
    const auto items = m_scene->items(scenePos, Qt::IntersectsItemShape, Qt::DescendingOrder);
    for (auto *it : items)
    {
        if (auto *b = dynamic_cast<DocumentBlockItem *>(it))
        {
            return b->documentId();
        }
    }
    return {};
}

void MatrixView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        const auto id = docIdAt(event->pos());
        if (id.isEmpty() == false)
        {
            emit blockDoubleClicked(id);
            event->accept();
            return;
        }
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}

void MatrixView::contextMenuEvent(QContextMenuEvent *event)
{
    const auto id = docIdAt(event->pos());
    if (id.isEmpty() == false)
    {
        emit blockContextMenuRequested(id, event->globalPos());
    }
    else
    {
        emit emptyContextMenuRequested(event->globalPos());
    }

    event->accept();
}

void MatrixView::wheelEvent(QWheelEvent *event)
{
    const double factor = event->angleDelta().y() > 0
                              ? 1.15
                              : 1.0 / 1.15;
    scale(factor, factor);
    event->accept();
}