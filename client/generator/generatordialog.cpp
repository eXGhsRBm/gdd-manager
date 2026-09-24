#include "generatordialog.h"
#include "ui_generatordialog.h"

#include "settings.h"
#include "template.h"
#include "project.h"
#include "loader.h"
#include "translation.h"
#include "generatortablemodel.h"
#include "toccombodelegate.h"

#include <QDir>
#include <QJsonArray>
#include <QFileDialog>
#include <QMessageBox>

GeneratorDialog::GeneratorDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GeneratorDialog)
{
    ui->setupUi(this);

    m_model = new GeneratorTableModel(this);
    ui->tableView->setModel(m_model);
    ui->tableView->setItemDelegateForColumn(GeneratorTableModel::ColToc, new TocComboDelegate(m_model, this));

    ui->tableView->horizontalHeader()->setSectionResizeMode(GeneratorTableModel::ColDocument, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(GeneratorTableModel::ColToc, QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setVisible(false);

    if (LAST_PROJECT_NAME.isEmpty() == false)
    {
        ui->lineEdit_projectName->setText(LAST_PROJECT_NAME);
    }

    if (LAST_ROOT_DIR.isEmpty() == false)
    {
        ui->lineEdit_root->setText(LAST_ROOT_DIR);
    }

    if (LAST_TEMPLATE_PATH.isEmpty() == false)
    {
        ui->lineEdit_template->setText(LAST_TEMPLATE_PATH);
    }

    if (LAST_LANG_FILE_PATH.isEmpty() == false)
    {
        ui->lineEdit_projectLanguage->setText(LAST_LANG_FILE_PATH);
    }

    reloadTable();
    restoreTocSelection();

    translation();
}

void GeneratorDialog::translation()
{
    setWindowTitle(tr("Generate documents"));

    ui->lineEdit_projectName->setPlaceholderText(tr("Project Name"));

    ui->lineEdit_root->setPlaceholderText(tr("Root folder"));
    ui->pushButton_openRoot->setText(tr("Open"));

    ui->lineEdit_template->setPlaceholderText(tr("Template"));
    ui->pushButton_openTemplate->setText(tr("Open"));

    ui->lineEdit_projectLanguage->setPlaceholderText(tr("Project language"));
    ui->pushButton_openProjectLanguage->setText(tr("Open"));

    ui->checkBox_divideChapters->setText(tr("Divide the chapters into files"));
    ui->checkBox_rewriteDocuments->setText(tr("Overwrite existing files"));

    ui->pushButton_generate->setText(tr("Generate"));

    if (m_model)
    {
        m_model->setHorizontalHeaderLabels({
            tr("Document"),
            tr("TOC")
        });
    }
}

GeneratorDialog::~GeneratorDialog()
{
    delete ui;
}

QString GeneratorDialog::tocDir() const
{
    return APP_DIR + QStringLiteral("/toc");
}

void GeneratorDialog::reloadTable()
{
    if (LAST_TEMPLATE_PATH.isEmpty() || QFile::exists(LAST_TEMPLATE_PATH) == false)
    {
        m_model->loadFromCatalog({}, tocDir());
        return;
    }

    const auto tmpl = Template::load(LAST_TEMPLATE_PATH);
    m_model->loadFromCatalog(tmpl.documents, tocDir());
}

void GeneratorDialog::restoreTocSelection()
{
    for (auto row = 0; row < m_model->rowCount(); ++row)
    {
        const auto docId = m_model->docIdForRow(row);
        if (LAST_TOC_PATHS.contains(docId) == false)
        {
            continue;
        }

        const auto savedPath = LAST_TOC_PATHS.value(docId);
        const auto tocFiles  = m_model->tocFilesForRow(row);
        if (tocFiles.contains(savedPath) == false)
        {
            continue;
        }

        const auto idx = m_model->index(row, GeneratorTableModel::ColToc);
        m_model->setData(idx, savedPath, Qt::EditRole);
    }
}

void GeneratorDialog::on_pushButton_openRoot_clicked()
{
    const auto dir = QFileDialog::getExistingDirectory(this,
                                                       tr("Root folder"));
    if (dir.isEmpty())
    {
        return;
    }

    ui->lineEdit_root->setText(dir);
    LAST_ROOT_DIR = dir;
    reloadTable();
}

void GeneratorDialog::on_pushButton_openTemplate_clicked()
{
    const auto file = QFileDialog::getOpenFileName(this,
                                                   tr("Template"),
                                                   {},
                                                   QStringLiteral("JSON (*.json)"));

    if (file.isEmpty() == false)
    {
        LAST_TEMPLATE_PATH = file;
        ui->lineEdit_template->setText(file);
        reloadTable();
    }
}

void GeneratorDialog::on_pushButton_generate_clicked()
{
    const auto projectName = ui->lineEdit_projectName->text().trimmed();
    if (projectName.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Project name is empty"));
        return;
    }

    const auto rootPath = ui->lineEdit_root->text().trimmed();
    if (rootPath.isEmpty() || QDir(rootPath).exists() == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Root folder is invalid"));
        return;
    }

    const auto templatePath = ui->lineEdit_template->text().trimmed();
    if (templatePath.isEmpty() || QFile::exists(templatePath) == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Template is invalid"));
        return;
    }

    const auto langPath = ui->lineEdit_projectLanguage->text().trimmed();
    if (langPath.isEmpty() || QFile::exists(langPath) == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Project language is invalid"));
        return;
    }

    if (Translation::instance().setProject(langPath) == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Failed to load project language"));
        return;
    }

    const auto divide = ui->checkBox_divideChapters->isChecked();
    auto overwrite = ui->checkBox_rewriteDocuments->isChecked();

    const auto tmpl = Template::load(templatePath);
    if (tmpl.documents.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Template has no documents"));
        return;
    }

    QMap<QString, QString> tocByDoc;
    for (auto row = 0; row < m_model->rowCount(); ++row)
    {
        const auto docId   = m_model->docIdForRow(row);
        const auto tocPath = m_model->selectedTocForRow(row);
        if (docId.isEmpty() == false && tocPath.isEmpty() == false)
        {
            tocByDoc.insert(docId, tocPath);
        }
    }

    const auto projectDir = rootPath + "/" + projectName;
    if (QDir(projectDir).exists() && QDir(projectDir).isEmpty() == false)
    {
        if (overwrite == false)
        {
            const auto reply = QMessageBox::question(this,
                                                     tr("Project exists"),
                                                     tr("Folder %1 is not empty. Overwrite?").arg(projectName));

            if (reply != QMessageBox::Yes)
            {
                return;
            }
            overwrite = true;
        }
    }
    QDir().mkpath(projectDir);

    ProjectData proj;
    proj.name         = projectName;
    proj.templatePath = templatePath;
    proj.createdAt    = QDateTime::currentDateTime().toString(Qt::ISODate);
    proj.language     = langPath;
    proj.categories   = tmpl.categories;
    proj.stages       = tmpl.stages;

    for (const auto &doc : tmpl.documents)
    {
        if (tocByDoc.contains(doc.id) == false)
        {
            continue;
        }

        ProjectDocument pd;
        pd.id       = doc.id;
        pd.category = doc.category;
        pd.stage    = doc.stage;
        pd.folder   = doc.id;
        pd.tocPath  = tocByDoc.value(doc.id);

        const auto docDir = projectDir + "/" + pd.folder;
        QDir().mkpath(docDir);

        const auto tocRoot = Loader::load(pd.tocPath);
        QStringList sections;
        for (const auto &v : tocRoot.value("sections").toArray())
        {
            sections << v.toString();
        }

        if (divide)
        {
            int idx = 1;
            for (const auto &secKey : sections)
            {
                const auto fname = QString("%1_%2_%3.md").arg(doc.id).arg(idx, 2, 10, QChar('0')).arg(secKey);
                const auto abs = docDir + "/" + fname;

                if (overwrite || QFile::exists(abs) == false)
                {
                    QFile f(abs);
                    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
                    {
                        QTextStream out(&f);
                        out.setEncoding(QStringConverter::Utf8);

                        const auto display = TRANSLATE_TOC(doc.id, secKey);
                        out << "# " << display << "\n\n";
                    }
                }
                pd.files << fname;
                ++idx;
            }
        }
        else
        {
            const auto fname = doc.id + ".md";
            const auto abs   = docDir + "/" + fname;

            if (overwrite || QFile::exists(abs) == false)
            {
                QFile f(abs);
                if (f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
                {
                    QTextStream out(&f);
                    out.setEncoding(QStringConverter::Utf8);
                    for (const auto &secKey : sections)
                    {
                        const auto display = TRANSLATE_TOC(doc.id, secKey);
                        out << "# " << display << "\n\n";
                    }
                }
            }
            pd.files << fname;
        }
        proj.documents.append(pd);
    }

    const auto jsonPath = Project::jsonPath(projectDir);
    if (Project::save(jsonPath, proj) == false)
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Failed to save project.json"));
        return;
    }

    LAST_PROJECT_NAME  = projectName;
    LAST_ROOT_DIR      = rootPath;
    LAST_TEMPLATE_PATH = templatePath;

    LAST_TOC_PATHS.clear();
    for (auto row = 0; row < m_model->rowCount(); ++row)
    {
        const auto docId   = m_model->docIdForRow(row);
        const auto tocPath = m_model->selectedTocForRow(row);
        if (docId.isEmpty() == false && tocPath.isEmpty() == false)
        {
            LAST_TOC_PATHS.insert(docId, tocPath);
        }
    }

    LAST_LANG_FILE_PATH = langPath;
    Settings::instance().save();
    accept();
}

void GeneratorDialog::on_pushButton_openProjectLanguage_clicked()
{
    const auto startDir = APP_DIR + "/languages/doc";
    const auto file = QFileDialog::getOpenFileName(this,
                                                   tr("Project language"),
                                                   startDir,
                                                   QStringLiteral("JSON (*.json)"));

    if (file.isEmpty())
    {
        return;
    }

    ui->lineEdit_projectLanguage->setText(file);
    LAST_LANG_FILE_PATH = file;
}

