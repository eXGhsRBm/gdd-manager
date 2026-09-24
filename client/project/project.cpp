#include "project.h"
#include "loader.h"

#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>

QString Project::jsonPath(const QString &projectDir)
{
    return projectDir + "/" + QFileInfo(projectDir).fileName() + ".json";
}

QString Project::projectDir(const QString &jsonPath)
{
    return QFileInfo(jsonPath).absolutePath();
}

ProjectData Project::load(const QString &jsonPath)
{
    ProjectData data;
    const auto root = Loader::load(jsonPath);
    if (root.isEmpty())
    {
        return data;
    }

    data.name         = root.value("name").toString();
    data.templatePath = root.value("template_path").toString();
    data.createdAt    = root.value("created_at").toString();
    data.language     = root.value("language").toString();

    for (const auto &v : root.value("categories").toArray())
    {
        const auto o = v.toObject();
        data.categories.append({
            o.value("id").toString(),
            QColor(o.value("border").toString("#6c8ebf"))
        });
    }

    for (const auto &v : root.value("stages").toArray())
    {
        const auto o = v.toObject();
        data.stages.append({
            o.value("id").toString(),
            QColor(o.value("border").toString("#000000"))
        });
    }

    for (const auto &v : root.value("documents").toArray())
    {
        const auto o = v.toObject();
        ProjectDocument d;
        d.id       = o.value("id").toString();
        d.category = o.value("category").toString();
        d.stage    = o.value("stage").toString();
        d.folder   = o.value("folder").toString();
        d.tocPath  = o.value("toc_path").toString();

        for (const auto &f : o.value("files").toArray())
        {
            d.files << f.toString();
        }
        data.documents.append(d);
    }
    return data;
}

bool Project::save(const QString &jsonPath, const ProjectData &data)
{
    QJsonObject root;
    root["name"]          = data.name;
    root["template_path"] = data.templatePath;
    root["created_at"]    = data.createdAt;
    root["language"]      = data.language;

    QJsonArray cats, stgs;
    for (const auto &c : data.categories)
    {
        cats.append(QJsonObject{ {"id", c.id}, {"border", c.border.name()} });
    }
    root["categories"] = cats;

    for (const auto &s : data.stages)
    {
        stgs.append(QJsonObject{ {"id", s.id}, {"border", s.border.name()} });
    }
    root["stages"]     = stgs;

    QJsonArray docs;
    for (const auto &d : data.documents)
    {
        QJsonArray files;
        for (const auto &f : d.files)
        {
            files.append(f);
        }
        docs.append(QJsonObject{
            {"id",       d.id},
            {"category", d.category},
            {"stage",    d.stage},
            {"folder",   d.folder},
            {"toc_path", d.tocPath},
            {"files",    files}
        });
    }
    root["documents"] = docs;

    return Loader::save(root, jsonPath);
}