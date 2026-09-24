#include "templatedialog.h"
#include "ui_templatedialog.h"

#include "categoriestablemodel.h"
#include "docstablemodel.h"

#include <QMenu>
#include <QMessageBox>

void TemplateDialog::on_pushButton_addCategory_clicked()
{
    m_catModel->addRow();
    const auto last = m_catModel->rowCount() - 1;
    if (last >= 0)
    {
        ui->tableView_categories->selectRow(last);
    }
}

void TemplateDialog::on_pushButton_saveCategories_clicked()
{
    saveTemplate();
}

void TemplateDialog::onTableCategoriesContextMenu(const QPoint &pos)
{
    if (m_isSample)
    {
        return;
    }

    const auto idx = ui->tableView_categories->indexAt(pos);
    if (idx.isValid() == false)
    {
        return;
    }

    QMenu menu;
    auto del    = menu.addAction(tr("Delete"));
    auto chosen = menu.exec(ui->tableView_categories->viewport()->mapToGlobal(pos));
    if (chosen == del)
    {
        confirmCategoryRemoval(idx.row());
    }
}

bool TemplateDialog::confirmCategoryRemoval(int row)
{
    const auto id = m_catModel->index(row, CategoriesTableModel::ColId).data(Qt::DisplayRole).toString();

    const auto deps = m_docModel->countByCategory(id);
    if (deps > 0)
    {
        QMessageBox::warning(this,
                             tr("Cannot delete"),
                             tr("Category \"%1\" is used in %2 document(s). "
                                "Reassign them first.").arg(id).arg(deps));
        return false;
    }

    m_catModel->removeRow(row);
    return true;
}