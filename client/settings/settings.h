#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QMap>

struct SettingsData {
    QString appDir;
    QString settingsFilePath;
    QString languageFilePath;

    QString lastProjectName;
    QString lastRootDir;
    QString lastTemplatePath;
    QString lastProjectLanguageFilePath;
    QMap<QString, QString> lastTocPaths;
};

class Settings
{
public:
    static Settings &instance();

    SettingsData data;

    void load();
    bool save();

private:
    Settings() = default;
    Settings(const Settings &) = delete;
    Settings &operator=(const Settings &) = delete;
};

inline QString& APP_DIR             = Settings::instance().data.appDir;
inline QString& SETTINGS_FILE_PATH  = Settings::instance().data.settingsFilePath;
inline QString& LANGUAGE_FILE_PATH  = Settings::instance().data.languageFilePath;

inline QString& LAST_PROJECT_NAME  = Settings::instance().data.lastProjectName;
inline QString& LAST_ROOT_DIR      = Settings::instance().data.lastRootDir;
inline QString& LAST_TEMPLATE_PATH = Settings::instance().data.lastTemplatePath;

inline QString& LAST_LANG_FILE_PATH = Settings::instance().data.lastProjectLanguageFilePath;

inline QMap<QString, QString>& LAST_TOC_PATHS = Settings::instance().data.lastTocPaths;

#define LOG qDebug() << qPrintable("[" + QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + "]") << Q_FUNC_INFO

#endif // SETTINGS_H
