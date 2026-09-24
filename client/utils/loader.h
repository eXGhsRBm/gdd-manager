#ifndef LOADER_H
#define LOADER_H

#include <QJsonObject>

class Loader
{
public:
    static QJsonObject load(const QString &path);

    static bool save(const QJsonObject &obj,
                     const QString &path);

    static bool extractIfMissing();

private:
    static bool extractFile(const QString &resourcePath,
                            const QString &targetPath);
};

#endif // LOADER_H
