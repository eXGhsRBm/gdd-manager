#include "template.h"
#include "loader.h"

#include <QJsonArray>
#include <QJsonObject>

static QColor colorOr(const QJsonObject &o,
                      const char *key,
                      const QColor &def)
{
    const auto s = o.value(key).toString();
    return s.isEmpty()
               ? def
               : QColor(s);
}

TemplateData Template::load(const QString &path)
{
    TemplateData data;

    const auto root = Loader::load(path);
    for (const auto &v : root.value("categories").toArray())
    {
        const auto o = v.toObject();
        data.categories.append({
            o.value("id").toString(),
            colorOr(o, "border", QColor("#6c8ebf"))
        });
    }

    for (const auto &v : root.value("stages").toArray())
    {
        const auto o = v.toObject();
        data.stages.append({
            o.value("id").toString(),
            colorOr(o, "border", QColor("#000000"))
        });
    }

    for (const auto &v : root.value("documents").toArray())
    {
        const auto o = v.toObject();
        data.documents.append({
            o.value("id").toString(),
            o.value("category").toString(),
            o.value("stage").toString()
        });
    }

    return data;
}

bool Template::save(const QString &path,
                    const TemplateData &data)
{
    QJsonArray cats;
    for (const auto &c : data.categories)
    {
        cats.append(QJsonObject{
            {"id",     c.id},
            {"border", c.border.name()}
        });
    }

    QJsonArray stgs;
    for (const auto &s : data.stages)
    {
        stgs.append(QJsonObject{
            {"id",     s.id},
            {"border", s.border.name()}
        });
    }

    QJsonArray docs;
    for (const auto &d : data.documents)
    {
        docs.append(QJsonObject{
            {"id",       d.id},
            {"category", d.category},
            {"stage",    d.stage}
        });
    }

    QJsonObject root;
    root["categories"] = cats;
    root["stages"]     = stgs;
    root["documents"]  = docs;

    return Loader::save(root, path);
}