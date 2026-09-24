#include "templatedialog.h"
#include "ui_templatedialog.h"
#include "docstablemodel.h"

#include <QMenu>

void TemplateDialog::on_pushButton_addDoc_clicked()
{
    m_docModel->addRow();
    const auto last = m_docModel->rowCount() - 1;
    if (last >= 0)
    {
        ui->tableView_docs->selectRow(last);
    }
}

void TemplateDialog::on_pushButton_saveDocs_clicked()
{
    saveTemplate();
}

void TemplateDialog::onTableDocsContextMenu(const QPoint &pos)
{
    if (m_isSample)
    {
        return;
    }

    const auto idx = ui->tableView_docs->indexAt(pos);
    if (idx.isValid() == false)
    {
        return;
    }

    QMenu menu;
    auto del    = menu.addAction(tr("Delete"));
    auto chosen = menu.exec(ui->tableView_docs->viewport()->mapToGlobal(pos));
    if (chosen == del)
    {
        m_docModel->removeRow(idx.row());
    }
}