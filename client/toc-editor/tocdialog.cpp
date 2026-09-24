#include "tocdialog.h"
#include "ui_tocdialog.h"

#include "loader.h"
#include "settings.h"
#include "sectionslistmodel.h"

#include <QDir>
#include <QFileInfo>
#include <QHeaderView>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QRegularExpression>
#include <QJsonArray>

QString TocDialog::tocDir() const
{
    return APP_DIR + QStringLiteral("/toc");
}

QString TocDialog::selectedTocPath() const
{
    const auto name = ui->comboBox_tocs->currentData().toString();
    if (name.isEmpty())
    {
        return {};
    }
    return tocDir() + "/" + name;
}

QString TocDialog::selectedTocId() const
{
    const auto path = selectedTocPath();
    if (path.isEmpty())
    {
        return {};
    }
    const auto root = Loader::load(path);
    return root.value("id").toString();
}

TocDialog::TocDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TocDialog)
{
    ui->setupUi(this);

    m_sectionsModel = new SectionsListModel(this);
    ui->listView_sections->setModel(m_sectionsModel);
    ui->listView_sections->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listView_sections->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

    connect(ui->listView_sections, &QListView::customContextMenuRequested, this, &TocDialog::onSectionsContextMenu);

    QDir().mkpath(tocDir());

    populateTocs();

    connect(ui->comboBox_tocs, &QComboBox::currentIndexChanged, this, &TocDialog::on_comboBox_tocs_currentIndexChanged);

    on_comboBox_tocs_currentIndexChanged(ui->comboBox_tocs->currentIndex());

    translation();
}

TocDialog::~TocDialog()
{
    delete ui;
}

void TocDialog::translation()
{
    setWindowTitle(tr("TOC editor"));

    ui->pushButton_addToc->setText(tr("Add"));
    ui->pushButton_addSection->setText(tr("Add section"));
    ui->pushButton_save->setText(tr("Save"));
    ui->pushButton_resetSectionChanges->setText(tr("Reset"));
}

void TocDialog::populateTocs()
{
    QSignalBlocker blocker(ui->comboBox_tocs);
    ui->comboBox_tocs->clear();

    QDir dir(tocDir());
    const auto files = dir.entryInfoList({"*.json"}, QDir::Files, QDir::Name);
    for (const auto &fi : files)
    {
        ui->comboBox_tocs->addItem(fi.fileName(), fi.fileName());
    }

    if (ui->comboBox_tocs->count() > 0)
    {
        ui->comboBox_tocs->setCurrentIndex(0);
    }
}

void TocDialog::loadTocFrom(const QString &path)
{
    const auto fileName = QFileInfo(path).fileName();
    m_isSample = isSample(fileName);
    if (m_isSample || path.isEmpty() || QFile::exists(path) == false)
    {
        m_savedSections.clear();
        m_sectionsModel->load({});
        updateActionsState();
        return;
    }

    const auto root = Loader::load(path);
    QStringList sections;
    for (const auto &v : root.value("sections").toArray())
    {
        sections << v.toString();
    }

    m_savedSections = sections;
    m_sectionsModel->load(sections);
    updateActionsState();
}

bool TocDialog::saveToc(const QString &path)
{
    if (m_isSample)
    {
        return false;
    }

    const auto id = selectedTocId();
    if (id.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("TOC has no \"id\" field"));
        return false;
    }

    const auto sections = m_sectionsModel->sections();
    QSet<QString> seen;
    QStringList duplicates;
    for (const auto &s : sections)
    {
        if (s.isEmpty())
        {
            continue;
        }

        if (seen.contains(s))
        {
            duplicates << s;
        }
        seen.insert(s);
    }

    if (duplicates.isEmpty() == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Duplicate sections: %1").arg(duplicates.join(", ")));
        return false;
    }

    QJsonArray arr;
    for (const auto &s : sections)
    {
        if (s.isEmpty() == false)
        {
            arr.append(s);
        }
    }

    QJsonObject root;
    root["id"]       = id;
    root["sections"] = arr;
    if (Loader::save(root, path) == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Failed to save TOC: %1").arg(path));
        return false;
    }

    m_savedSections = sections;
    return true;
}

void TocDialog::on_comboBox_tocs_currentIndexChanged(int)
{
    loadTocFrom(selectedTocPath());
}

void TocDialog::on_pushButton_addToc_clicked()
{
    const auto sourcePath = selectedTocPath();
    if (sourcePath.isEmpty() || QFile::exists(sourcePath) == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("No TOC selected"));
        return;
    }

    auto ok = false;
    const auto name = QInputDialog::getText(this,
                                            tr("New TOC"),
                                            tr("File name:"),
                                            QLineEdit::Normal,
                                            QStringLiteral("new_toc"),
                                            &ok);

    if (ok == false || name.trimmed().isEmpty())
    {
        return;
    }

    auto safe = name.trimmed();
    safe.replace(QRegularExpression(R"([^\w\-]+)"), "_");
    if (safe.endsWith(".json") == false)
    {
        safe += ".json";
    }

    if (isSample(safe))
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Name must not start with \"sample\""));
        return;
    }

    const auto path = tocDir() + "/" + safe;
    if (QFile::exists(path))
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("File already exists: %1").arg(safe));
        return;
    }

    const auto source = Loader::load(sourcePath);
    if (Loader::save(source, path) == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Failed to create TOC"));
        return;
    }

    populateTocs();
    const auto idx = ui->comboBox_tocs->findData(safe);
    if (idx >= 0)
    {
        ui->comboBox_tocs->setCurrentIndex(idx);
    }

    loadTocFrom(path);
}

void TocDialog::on_pushButton_addSection_clicked()
{
    m_sectionsModel->addRow();
    const auto last = m_sectionsModel->rowCount() - 1;
    if (last >= 0)
    {
        const auto idx = m_sectionsModel->index(last, 0);
        ui->listView_sections->setCurrentIndex(idx);
        ui->listView_sections->edit(idx);
    }
}

void TocDialog::on_pushButton_save_clicked()
{
    const auto path = selectedTocPath();
    if (path.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("No TOC selected"));
        return;
    }

    if (saveToc(path))
    {
        QMessageBox::information(this,
                                 tr("Done"),
                                 tr("TOC saved: %1").arg(QFileInfo(path).fileName()));
    }
}

void TocDialog::onSectionsContextMenu(const QPoint &pos)
{
    if (m_isSample)
    {
        return;
    }

    const auto idx = ui->listView_sections->indexAt(pos);
    if (idx.isValid() == false)
    {
        return;
    }

    QMenu menu;
    auto actEdit   = menu.addAction(tr("Edit"));
    auto actDelete = menu.addAction(tr("Delete"));
    auto chosen    = menu.exec(ui->listView_sections->viewport()->mapToGlobal(pos));
    if (chosen == actEdit)
    {
        ui->listView_sections->edit(idx);
    }
    else if (chosen == actDelete)
    {
        m_sectionsModel->removeRow(idx.row());
    }
}

void TocDialog::on_pushButton_resetSectionChanges_clicked()
{
    m_sectionsModel->load(m_savedSections);
}

bool TocDialog::isSample(const QString &fileName) const
{
    return QFileInfo(fileName).fileName().startsWith(QStringLiteral("sample"), Qt::CaseInsensitive);
}

void TocDialog::updateActionsState()
{
    const auto editing = !m_isSample;

    ui->pushButton_addSection->setEnabled(editing);
    ui->pushButton_save->setEnabled(editing);
    ui->pushButton_resetSectionChanges->setEnabled(editing);
}