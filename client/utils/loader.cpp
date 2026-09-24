#include "loader.h"

#include "settings.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonParseError>

QJsonObject Loader::load(const QString &path)
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly))
    {
        const auto bytes = file.readAll();
        file.close();

        if (bytes.isEmpty())
        {
            return {};
        }

        QJsonParseError pe;
        const auto doc = QJsonDocument::fromJson(bytes, &pe);
        if (pe.error == QJsonParseError::NoError)
        {
            if (doc.isObject())
            {
                return doc.object();
            }
        }
    }
    return {};
}

bool Loader::save(const QJsonObject& obj,
                  const QString &path)
{
    const QFileInfo fi(path);
    const auto dir = fi.absoluteDir();
    if (dir.exists() == false && dir.mkpath(".") == false)
    {
        return false;
    }

    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        const auto bytes = QJsonDocument(obj).toJson(QJsonDocument::Indented);
        file.write(bytes);
        file.close();
        return true;
    }
    return false;
}

bool Loader::extractFile(const QString &resourcePath,
                         const QString &targetPath)
{
    QFile src(resourcePath);
    if (src.open(QIODevice::ReadOnly) == false)
    {
        return false;
    }

    const QFileInfo fi(targetPath);
    QDir().mkpath(fi.absolutePath());

    QFile dst(targetPath);
    if (dst.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
    {
        return false;
    }

    dst.write(src.readAll());
    return true;
}

bool Loader::extractIfMissing()
{
    const QString prefix = ":/resources/";
    QDirIterator it(prefix, QDirIterator::Subdirectories);
    auto ok = true;

    while (it.hasNext())
    {
        it.next();
        const auto fi = it.fileInfo();
        if (fi.isFile() == false)
        {
            continue;
        }

        const auto rel = fi.absoluteFilePath().mid(prefix.length());
        const auto target = APP_DIR  + "/" + rel;

        if (QFile::exists(target))
        {
            continue;
        }

        if (extractFile(fi.absoluteFilePath(), target) == false)
        {
            ok = false;
        }
    }
    return ok;
}