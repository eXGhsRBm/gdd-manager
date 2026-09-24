#include "mainwindow.h"

#include "editor.h"
#include "theme.h"
#include "loader.h"
#include "settings.h"
#include "translation.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    APP_DIR = QCoreApplication::applicationDirPath();

    Loader::extractIfMissing();

    Settings::instance().load();
    Theme::instance().load();
    Editor::instance().load();

    Translation::instance().setLanguage(LANGUAGE_FILE_PATH);

    MainWindow w;
    w.show();

    return QApplication::exec();
}
