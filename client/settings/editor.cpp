#include "editor.h"

#include "loader.h"
#include "settings.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>

namespace {

const QString EDITORS_REL = "/settings/editors.json";
const QString QRC_EDITORS = ":/resources/settings/editors.json";

EditorData readEditor(const QJsonObject &o)
{
    EditorData e;
    e.name = o.value("name").toString();
    e.path = o.value("path").toString();
    e.args = o.value("args").toString();
    return e;
}

QJsonObject writeEditor(const EditorData &e)
{
    QJsonObject o;
    o["name"] = e.name;
    o["path"] = e.path;
    o["args"] = e.args;
    return o;
}

} // namespace

Editor &Editor::instance()
{
    static Editor s;
    return s;
}

void Editor::load()
{
    const auto path = APP_DIR + EDITORS_REL;

    QJsonObject root;
    if (QFile::exists(path))
    {
        root = Loader::load(path);
    }
    else
    {
        root = Loader::load(QRC_EDITORS);
        Loader::save(root, path);
    }

    m_editors.clear();
    for (const auto &v : root.value("editors").toArray())
    {
        const auto o = v.toObject();
        const auto e = readEditor(o);
        if (e.name.isEmpty() == false)
        {
            m_editors.insert(e.name, e);
        }
    }

    m_default = root.value("editor_default").toString();
    if (m_editors.contains(m_default) == false)
    {
        m_default = m_editors.isEmpty()
                        ? QString()
                        : m_editors.firstKey();
    }
}

bool Editor::save()
{
    if (m_editors.contains(m_default) == false)
    {
        m_default = m_editors.isEmpty() ? QString() : m_editors.firstKey();
    }

    QJsonObject root;
    root["editor_default"] = m_default;

    QJsonArray arr;
    for (auto it = m_editors.begin(); it != m_editors.end(); ++it)
    {
        arr.append(writeEditor(it.value()));
    }
    root["editors"] = arr;

    return Loader::save(root, APP_DIR + EDITORS_REL);
}

EditorData Editor::get(const QString &name) const
{
    return m_editors.value(name);
}

void Editor::set(const QString &name, const EditorData &data)
{
    auto e = data;
    e.name = name;
    m_editors.insert(name, e);
}

void Editor::setDefault(const QString &name)
{
    if (m_editors.contains(name))
    {
        m_default = name;
    }
}