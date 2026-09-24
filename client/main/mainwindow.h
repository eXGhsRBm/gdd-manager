#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "translator.h"
#include "project.h"

#include <QMainWindow>

class MatrixWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onOpenProject();
    void onGenerator();
    void onScanner();
    void onSearch();
    void onSettings();

    void openTemplate();
    void openToc();

    void applyTheme();
    void onProjectModified();

private:
    Ui::MainWindow *ui;
    MatrixWidget *m_matrix = nullptr;

    void translation();

    QString m_projectJsonPath;
    ProjectData m_projectData;
};

#endif // MAINWINDOW_H