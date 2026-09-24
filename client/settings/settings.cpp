#include "settings.h"
#include "loader.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonObject>

Settings &Settings::instance()
{
    static Settings s;
    return s;
}

void Settings::load()
{
    SETTINGS_FILE_PATH = APP_DIR + "/settings/settings.json";

    QJsonObject root;
    if (QFile::exists(SETTINGS_FILE_PATH))
    {
        root = Loader::load(SETTINGS_FILE_PATH);
    }
    else
    {
        root = Loader::load(":/resources/settings/settings.json");
        Loader::save(root, SETTINGS_FILE_PATH);
    }

    const auto lang = root.value("language_file_path").toString();
    LANGUAGE_FILE_PATH = lang.isEmpty()
                             ? APP_DIR + "/languages/ui/english.json"
                             : APP_DIR + "/" + lang;

    LAST_PROJECT_NAME = root.value("last_project_name").toString();

    const auto lastRootDir = root.value("last_root_dir").toString();
    if (lastRootDir.isEmpty() == false && QDir(lastRootDir).exists())
    {
        LAST_ROOT_DIR = lastRootDir;
    }
    else
    {
        LAST_ROOT_DIR = APP_DIR + "/projects";
    }

    const auto lastTemplatePath = root.value("last_template_path").toString();
    if (lastTemplatePath.isEmpty() == false && QFile::exists(lastTemplatePath))
    {
        LAST_TEMPLATE_PATH = lastTemplatePath;
    }
    else
    {
        LAST_TEMPLATE_PATH = APP_DIR + "/templates/sample_template.json";
    }

    const auto lastProjectLanguageFilePath = root.value("last_project_language_file_path").toString();
    if (lastProjectLanguageFilePath.isEmpty() == false && QFile::exists(lastProjectLanguageFilePath))
    {
        LAST_LANG_FILE_PATH = lastProjectLanguageFilePath;
    }
    else
    {
        LAST_LANG_FILE_PATH = APP_DIR + "/languages/doc/english.json";
    }

    LAST_TOC_PATHS.clear();
    const auto tocObj = root.value("last_toc_paths").toObject();
    for (auto it = tocObj.begin(); it != tocObj.end(); ++it)
    {
        const auto path = it.value().toString();
        if (path.isEmpty() == false && QFile::exists(path))
        {
            LAST_TOC_PATHS.insert(it.key(), path);
        }
    }
}

bool Settings::save()
{
    QJsonObject root;

    if (LANGUAGE_FILE_PATH.isEmpty() == false)
    {
        root["language_file_path"] = QDir(APP_DIR).relativeFilePath(LANGUAGE_FILE_PATH);
    }

    if (LAST_PROJECT_NAME.isEmpty() == false)
    {
        root["last_project_name"] = LAST_PROJECT_NAME;
    }

    if (LAST_ROOT_DIR.isEmpty() == false)
    {
        root["last_root_dir"] = LAST_ROOT_DIR;
    }

    if (LAST_TEMPLATE_PATH.isEmpty() == false)
    {
        root["last_template_path"] = LAST_TEMPLATE_PATH;
    }

    if (LAST_LANG_FILE_PATH.isEmpty() == false)
    {
        root["last_project_language_file_path"] = LAST_LANG_FILE_PATH;
    }

    QJsonObject tocObj;
    for (auto it = LAST_TOC_PATHS.begin(); it != LAST_TOC_PATHS.end(); ++it)
    {
        tocObj[it.key()] = it.value();
    }
    root["last_toc_paths"] = tocObj;

    return Loader::save(root, SETTINGS_FILE_PATH);
}