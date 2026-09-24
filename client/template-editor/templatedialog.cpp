#include "templatedialog.h"
#include "ui_templatedialog.h"

#include "loader.h"
#include "settings.h"
#include "categoriestablemodel.h"
#include "stagestablemodel.h"
#include "docstablemodel.h"
#include "categorycolorbuttondelegate.h"
#include "stagecolorbuttondelegate.h"
#include "categorycombodelegate.h"
#include "stagecombodelegate.h"

#include <QDir>
#include <QHeaderView>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>

TemplateDialog::TemplateDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TemplateDialog)
{
    ui->setupUi(this);

    m_catModel   = new CategoriesTableModel(this);
    m_stageModel = new StagesTableModel(this);
    m_docModel   = new DocsTableModel(this);

    setupTables();
    setupDelegates();

    QDir().mkpath(templatesDir());

    populateTemplates();
    connect(ui->comboBox_templates, &QComboBox::currentIndexChanged, this, &TemplateDialog::on_comboBox_templates_currentIndexChanged);

    on_comboBox_templates_currentIndexChanged(ui->comboBox_templates->currentIndex());

    translation();
}

TemplateDialog::~TemplateDialog() { delete ui; }

void TemplateDialog::translation()
{
    setWindowTitle(tr("Template editor"));

    ui->tabWidget->setTabText(0, tr("Categories"));
    ui->tabWidget->setTabText(1, tr("Stages"));
    ui->tabWidget->setTabText(2, tr("Documents"));

    ui->pushButton_addCategory->setText(tr("Add category"));
    ui->pushButton_saveCategories->setText(tr("Save"));
    ui->pushButton_resetCategoryChanges->setText(tr("Reset"));

    ui->pushButton_addStage->setText(tr("Add stage"));
    ui->pushButton_saveStages->setText(tr("Save"));
    ui->pushButton_resetStageChanges->setText(tr("Reset"));

    ui->pushButton_addDoc->setText(tr("Add document"));
    ui->pushButton_saveDocs->setText(tr("Save"));
    ui->pushButton_resetDocChanges->setText(tr("Reset"));

    if (m_catModel)
    {
        m_catModel->setHorizontalHeaderLabels({
            tr("Category ID"),
            tr("Color")
        });
    }

    if (m_stageModel)
    {
        m_stageModel->setHorizontalHeaderLabels({
            tr("Stage ID"),
            tr("Color")
        });
    }

    if (m_docModel)
    {
        m_docModel->setHorizontalHeaderLabels({
            tr("Document ID"),
            tr("Category"),
            tr("Stage")
        });
    }
}

void TemplateDialog::setupTables()
{
    ui->tableView_categories->setModel(m_catModel);
    ui->tableView_categories->horizontalHeader()->setSectionResizeMode(CategoriesTableModel::ColId, QHeaderView::Stretch);
    ui->tableView_categories->horizontalHeader()->setSectionResizeMode(CategoriesTableModel::ColColor, QHeaderView::Fixed);
    ui->tableView_categories->setColumnWidth(CategoriesTableModel::ColColor, 80);
    ui->tableView_categories->verticalHeader()->setVisible(false);
    ui->tableView_categories->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->tableView_stages->setModel(m_stageModel);
    ui->tableView_stages->horizontalHeader()->setSectionResizeMode(StagesTableModel::ColId, QHeaderView::Stretch);
    ui->tableView_stages->horizontalHeader()->setSectionResizeMode(StagesTableModel::ColColor, QHeaderView::Fixed);
    ui->tableView_stages->setColumnWidth(StagesTableModel::ColColor, 80);
    ui->tableView_stages->verticalHeader()->setVisible(false);
    ui->tableView_stages->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->tableView_docs->setModel(m_docModel);
    ui->tableView_docs->horizontalHeader()->setSectionResizeMode(DocsTableModel::ColId, QHeaderView::Stretch);
    ui->tableView_docs->horizontalHeader()->setSectionResizeMode(DocsTableModel::ColCategory, QHeaderView::Stretch);
    ui->tableView_docs->horizontalHeader()->setSectionResizeMode(DocsTableModel::ColStage, QHeaderView::Stretch);
    ui->tableView_docs->verticalHeader()->setVisible(false);
    ui->tableView_docs->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->tableView_categories, &QTableView::customContextMenuRequested, this, &TemplateDialog::onTableCategoriesContextMenu);
    connect(ui->tableView_stages,     &QTableView::customContextMenuRequested, this, &TemplateDialog::onTableStagesContextMenu);
    connect(ui->tableView_docs,       &QTableView::customContextMenuRequested, this, &TemplateDialog::onTableDocsContextMenu);
}

void TemplateDialog::setupDelegates()
{
    ui->tableView_categories->setItemDelegateForColumn(CategoriesTableModel::ColColor, new CategoryColorButtonDelegate(this));
    ui->tableView_stages->setItemDelegateForColumn(StagesTableModel::ColColor, new StageColorButtonDelegate(this));
    ui->tableView_docs->setItemDelegateForColumn(DocsTableModel::ColCategory, new CategoryComboDelegate(m_catModel, this));
    ui->tableView_docs->setItemDelegateForColumn(DocsTableModel::ColStage, new StageComboDelegate(m_stageModel, this));
}

void TemplateDialog::saveTemplate()
{
    if (m_isSample)
    {
        return;
    }

    TemplateData data;
    data.categories = m_catModel->items();
    data.stages     = m_stageModel->items();
    data.documents  = m_docModel->items();

    QStringList errors;
    QSet<QString> catIds;
    for (const auto &c : data.categories)
    {
        if (c.id.isEmpty())
        {
            errors << tr("Category with empty ID");
            continue;
        }

        if (catIds.contains(c.id))
        {
            errors << tr("Duplicate category ID: %1").arg(c.id);
        }

        catIds.insert(c.id);
    }

    QSet<QString> stageIds;
    for (const auto &s : data.stages)
    {
        if (s.id.isEmpty())
        {
            errors << tr("Stage with empty ID");
            continue;
        }

        if (stageIds.contains(s.id))
        {
            errors << tr("Duplicate stage ID: %1").arg(s.id);
        }

        stageIds.insert(s.id);
    }

    QSet<QString> docIds;
    for (const auto &d : data.documents)
    {
        if (d.id.isEmpty())
        {
            errors << tr("Document with empty ID");
            continue;
        }

        if (docIds.contains(d.id))
        {
            errors << tr("Duplicate document ID: %1").arg(d.id);
        }
        docIds.insert(d.id);

        if (catIds.contains(d.category) == false)
        {
            errors << tr("Document \"%1\" refers to unknown category \"%2\"").arg(d.id, d.category);
        }

        if (stageIds.contains(d.stage) == false)
        {
            errors << tr("Document \"%1\" refers to unknown stage \"%2\"").arg(d.id, d.stage);
        }
    }

    if (errors.isEmpty() == false)
    {
        QMessageBox::warning(this,
                             tr("Validation error"),
                             errors.join("\n"));
        return;
    }

    const auto path = selectedTemplatePath();
    if (path.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("No template selected"));
        return;
    }

    if (Template::save(path, data) == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Failed to save template: %1").arg(path));
        return;
    }

    m_savedData = data;

    QMessageBox::information(this,
                             tr("Done"),
                             tr("Template saved: %1").arg(QFileInfo(path).fileName()));
}

QString TemplateDialog::templatesDir() const
{
    return APP_DIR + "/templates";
}

QString TemplateDialog::selectedTemplatePath() const
{
    const auto name = ui->comboBox_templates->currentData().toString();
    if (name.isEmpty())
    {
        return {};
    }
    return templatesDir() + "/" + name;
}

void TemplateDialog::populateTemplates()
{
    QSignalBlocker blocker(ui->comboBox_templates);
    ui->comboBox_templates->clear();

    QDir dir(templatesDir());
    const auto files = dir.entryInfoList({"*.json"}, QDir::Files, QDir::Name);
    for (const auto &fi : files)
    {
        ui->comboBox_templates->addItem(fi.fileName(), fi.fileName());
    }

    const auto current = LAST_TEMPLATE_PATH.isEmpty()
                             ? QString()
                             : QFileInfo(LAST_TEMPLATE_PATH).fileName();
    const auto idx = ui->comboBox_templates->findData(current);
    if (idx >= 0)
    {
        ui->comboBox_templates->setCurrentIndex(idx);
    }
    else if (ui->comboBox_templates->count() > 0)
    {
        ui->comboBox_templates->setCurrentIndex(0);
    }
}

void TemplateDialog::loadTemplateFrom(const QString &path)
{
    const auto fileName = QFileInfo(path).fileName();
    m_isSample = isSample(fileName);

    if (m_isSample || path.isEmpty() || !QFile::exists(path))
    {
        m_savedData = {};
        applyData({});
        updateActionsState();
        return;
    }

    const auto data = Template::load(path);
    m_savedData = data;
    applyData(data);
    updateActionsState();
}

void TemplateDialog::on_pushButton_addTemplate_clicked()
{
    const auto sourcePath = selectedTemplatePath();
    if (sourcePath.isEmpty() || QFile::exists(sourcePath) == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("No template selected"));
        return;
    }

    auto ok = false;
    const auto name = QInputDialog::getText(this,
                                            tr("New template"),
                                            tr("Template name:"),
                                            QLineEdit::Normal,
                                            QStringLiteral("new_template"),
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

    const auto path = templatesDir() + "/" + safe;
    if (QFile::exists(path))
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Template already exists: %1").arg(safe));
        return;
    }

    const auto source = Template::load(sourcePath);
    if (Template::save(path, source) == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Failed to create template"));
        return;
    }

    populateTemplates();
    const auto idx = ui->comboBox_templates->findData(safe);
    if (idx >= 0)
    {
        ui->comboBox_templates->setCurrentIndex(idx);
    }

    loadTemplateFrom(path);
}

void TemplateDialog::on_comboBox_templates_currentIndexChanged(int)
{
    const auto path = selectedTemplatePath();
    if (path.isEmpty())
    {
        return;
    }

    loadTemplateFrom(path);
    LAST_TEMPLATE_PATH = path;
}


void TemplateDialog::on_pushButton_resetCategoryChanges_clicked()
{
    m_catModel->load(m_savedData.categories);
}

void TemplateDialog::on_pushButton_resetStageChanges_clicked()
{
    m_stageModel->load(m_savedData.stages);
}

void TemplateDialog::on_pushButton_resetDocChanges_clicked()
{
    m_docModel->load(m_savedData.documents);
}

bool TemplateDialog::isSample(const QString &fileName) const
{
    return QFileInfo(fileName).fileName().startsWith(QStringLiteral("sample"), Qt::CaseInsensitive);
}

void TemplateDialog::updateActionsState()
{
    const auto editing = !m_isSample;

    ui->pushButton_addCategory->setEnabled(editing);
    ui->pushButton_saveCategories->setEnabled(editing);
    ui->pushButton_resetCategoryChanges->setEnabled(editing);

    ui->pushButton_addStage->setEnabled(editing);
    ui->pushButton_saveStages->setEnabled(editing);
    ui->pushButton_resetStageChanges->setEnabled(editing);

    ui->pushButton_addDoc->setEnabled(editing);
    ui->pushButton_saveDocs->setEnabled(editing);
    ui->pushButton_resetDocChanges->setEnabled(editing);
}

void TemplateDialog::applyData(const TemplateData &data)
{
    m_catModel->load(data.categories);
    m_stageModel->load(data.stages);
    m_docModel->load(data.documents);
}
