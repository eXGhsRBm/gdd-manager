#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <QColor>
#include <QString>
#include <QStringList>
#include <QVector>

struct CategoryInfo {
    QString id;
    QColor  border;
};

struct StageInfo {
    QString id;
    QColor  border;
};

struct DocumentInfo {
    QString id;
    QString category;
    QString stage;
};

struct TemplateData {
    QVector<CategoryInfo> categories;
    QVector<StageInfo>    stages;
    QVector<DocumentInfo> documents;
};

class Template
{
public:
    static TemplateData load(const QString &path);
    static bool save(const QString &path,
                     const TemplateData &data);
};

#endif