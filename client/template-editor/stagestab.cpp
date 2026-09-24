#include "templatedialog.h"
#include "ui_templatedialog.h"

#include "stagestablemodel.h"
#include "docstablemodel.h"

#include <QMenu>
#include <QMessageBox>

void TemplateDialog::on_pushButton_addStage_clicked()
{
    m_stageModel->addRow();
    const auto last = m_stageModel->rowCount() - 1;
    if (last >= 0)
    {
        ui->tableView_stages->selectRow(last);
    }
}

void TemplateDialog::on_pushButton_saveStages_clicked()
{
    saveTemplate();
}

void TemplateDialog::onTableStagesContextMenu(const QPoint &pos)
{
    if (m_isSample)
    {
        return;
    }

    const auto idx = ui->tableView_stages->indexAt(pos);
    if (idx.isValid() == false)
    {
        return;
    }

    QMenu menu;
    auto del    = menu.addAction(tr("Delete"));
    auto chosen = menu.exec(ui->tableView_stages->viewport()->mapToGlobal(pos));
    if (chosen == del)
    {
        confirmStageRemoval(idx.row());
    }
}

bool TemplateDialog::confirmStageRemoval(int row)
{
    const auto id = m_stageModel->index(row, StagesTableModel::ColId).data(Qt::DisplayRole).toString();
    const auto deps = m_docModel->countByStage(id);
    if (deps > 0)
    {
        QMessageBox::warning(this,
                             tr("Cannot delete"),
                             tr("Stage \"%1\" is used in %2 document(s). "
                                "Reassign them first.").arg(id).arg(deps));
        return false;
    }

    m_stageModel->removeRow(row);
    return true;
}