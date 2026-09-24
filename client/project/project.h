#ifndef PROJECT_H
#define PROJECT_H

#include "template.h"

#include <QString>
#include <QStringList>
#include <QVector>

struct ProjectDocument {
    QString id;
    QString category;
    QString stage;
    QString folder;
    QString tocPath;
    QStringList files;
};

struct ProjectData {
    QString name;
    QString templatePath;
    QString createdAt;
    QString language;
    QVector<CategoryInfo>    categories;
    QVector<StageInfo>       stages;
    QVector<ProjectDocument> documents;
};

class Project
{
public:
    static QString jsonPath(const QString &projectDir);
    static QString projectDir(const QString &jsonPath);

    static ProjectData load(const QString &jsonPath);
    static bool save(const QString &jsonPath,
                     const ProjectData &data);
};

#endif