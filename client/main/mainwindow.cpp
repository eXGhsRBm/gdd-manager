#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "generatordialog.h"
#include "scannerdialog.h"
#include "searchdialog.h"
#include "settingsdialog.h"
#include "templatedialog.h"
#include "tocdialog.h"
#include "settings.h"
#include "matrixwidget.h"
#include "matrixview.h"
#include "translation.h"

#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    translation();

    ui->toolBar->addAction(tr("Open project"), this, &MainWindow::onOpenProject);
    ui->toolBar->addAction(tr("Generator"),    this, &MainWindow::onGenerator);
    ui->toolBar->addAction(tr("Scanner"),      this, &MainWindow::onScanner);
    ui->toolBar->addAction(tr("Search"),       this, &MainWindow::onSearch);
    ui->toolBar->addAction(tr("Settings"),     this, &MainWindow::onSettings);

    auto *menu = new QMenu(tr("Documentation"), this);
    menu->addAction(tr("Templates"), this, &MainWindow::openTemplate);
    menu->addAction(tr("TOC"),       this, &MainWindow::openToc);
    ui->toolBar->addAction(menu->menuAction());

    m_matrix = new MatrixWidget(this);
    setCentralWidget(m_matrix);

    connect(m_matrix, &MatrixWidget::projectModified, this, &MainWindow::onProjectModified);

    applyTheme();
}

void MainWindow::translation()
{
    setWindowTitle(tr("GDD Manager"));
}

void MainWindow::applyTheme()
{
    const auto t = Theme::instance().current();

    const auto bg     = t.programBgColor;
    const auto bgDark = bg.darker(115);
    const auto bgDeep = bg.darker(130);
    const auto border = t.programBorderColor.isValid()
                            ? t.programBorderColor
                            : bg.darker(130);
    const auto fg     = t.programTextColor;
    const auto hover  = (bg.lightness() > 128)
                           ? bg.darker(110)
                           : bg.lighter(120);

    qApp->setStyleSheet(QString(R"(
        QWidget {
            background-color: %1;
            color: %2;
            font-size: %3pt;
        }
        QPushButton {
            padding: 4px 12px;
            background-color: %4;
            border: 1px solid %5;
            border-radius: 3px;
        }
        QPushButton:hover   { background-color: %8; }
        QPushButton:pressed { background-color: %6; }

        QLineEdit, QComboBox, QSpinBox,
        QTableView, QTreeView, QPlainTextEdit {
            background-color: %4;
            color: %2;
            border: 1px solid %5;
        }
        QHeaderView::section {
            background-color: %4;
            color: %2;
            border: 1px solid %5;
        }
        QTabWidget::pane {
            background-color: %1;
            border: 1px solid %5;
            top: -1px;
        }
        QTabBar { background-color: %1; }
        QTabBar::tab {
            background-color: %4;
            color: %2;
            padding: 6px 14px;
            border: 1px solid %5;
            border-bottom: none;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
            margin-right: 2px;
        }
        QTabBar::tab:selected {
            background-color: %1;
            color: %2;
            border-bottom: 1px solid %1;
        }
        QTabBar::tab:hover:!selected {
            background-color: %8;
        }
    )")
                            .arg(bg.name())
                            .arg(fg.name())
                            .arg(t.programTextSize)
                            .arg(bgDark.name())
                            .arg(border.name())
                            .arg(bgDeep.name())
                            .arg(hover.name()));

    if (m_matrix && m_matrix->view())
    {
        m_matrix->view()->setBackgroundBrush(QBrush(t.matrixBgColor));
    }

    if (m_projectData.name.isEmpty() == false && m_matrix)
    {
        m_matrix->setProject(m_projectData, m_projectJsonPath);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onOpenProject()
{
    QString startDir;

    if (m_projectJsonPath.isEmpty() == false)
    {
        startDir = QFileInfo(m_projectJsonPath).absolutePath();
    }
    else if (LAST_ROOT_DIR.isEmpty() == false && QDir(LAST_ROOT_DIR).exists())
    {
        startDir = LAST_ROOT_DIR;
    }
    else
    {
        startDir = QDir::homePath();
    }

    const auto file = QFileDialog::getOpenFileName(this,
                                                   tr("Open project"),
                                                   startDir,
                                                   QStringLiteral("Project (*.json)"));

    if (file.isEmpty())
    {
        return;
    }

    const auto data = Project::load(file);
    if (data.name.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Failed to load project: %1").arg(file));
        return;
    }

    m_projectJsonPath = file;
    m_projectData     = data;
    if (data.language.isEmpty() == false && QFile::exists(data.language))
    {
        Translation::instance().setProject(data.language);
    }
    else
    {
        Translation::instance().resetProject();
    }

    m_matrix->setProject(data, file);

    setWindowTitle(tr("GDD Manager") + " — " + data.name);
}

void MainWindow::onGenerator()
{
    GeneratorDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
        const auto projectDir = LAST_ROOT_DIR + "/" + LAST_PROJECT_NAME;
        const auto jsonPath = Project::jsonPath(projectDir);
        if (QFile::exists(jsonPath))
        {
            const auto data = Project::load(jsonPath);
            m_projectData = data;
            m_projectJsonPath = jsonPath;
            m_matrix->setProject(data, jsonPath);
            setWindowTitle(tr("GDD Manager") + " — " + data.name);
        }
        else
        {
            QMessageBox::warning(this,
                                 tr("Error"),
                                 tr("Generated project not found: %1").arg(jsonPath));
        }
    }
}

void MainWindow::onScanner()
{
    ScannerDialog dlg(this);
    if (m_projectJsonPath.isEmpty() == false)
    {
        dlg.setProjectDir(QFileInfo(m_projectJsonPath).absolutePath());
        dlg.setProjectJsonPath(m_projectJsonPath);
    }
    dlg.exec();
}

void MainWindow::onSearch()
{
    SearchDialog dlg(this);
    if (m_projectJsonPath.isEmpty() == false)
    {
        dlg.setProjectDir(QFileInfo(m_projectJsonPath).absolutePath());
    }
    dlg.exec();
}

void MainWindow::onSettings()
{
    SettingsDialog dlg(this);
    connect(&dlg, &SettingsDialog::themeChanged, this, &MainWindow::applyTheme);
    dlg.exec();
}

void MainWindow::openTemplate()
{
    TemplateDialog dlg(this);
    dlg.exec();
}

void MainWindow::openToc()
{
    TocDialog dlg(this);
    dlg.exec();
}

void MainWindow::onProjectModified()
{
    if (m_projectData.name.isEmpty() == false)
    {
        setWindowTitle(tr("GDD Manager") + " — " + m_projectData.name);
    }
}