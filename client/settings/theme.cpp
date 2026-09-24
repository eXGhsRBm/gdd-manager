#include "theme.h"

#include "loader.h"
#include "settings.h"

#include <QFile>
#include <QJsonObject>

namespace {

const QString THEMES_REL = "/settings/themes.json";
const QString QRC_THEMES = ":/resources/settings/themes.json";

ThemeData readTheme(const QString &name, const QJsonObject &o)
{
    ThemeData t;
    t.name             = name;
    t.programTextSize  = o.value("program_text_size").toInt(10);
    t.programTextColor = QColor(o.value("program_text_color").toString("#000000"));
    t.programBgColor   = QColor(o.value("program_bg_color").toString("#ffffff"));
    t.matrixBgColor    = QColor(o.value("matrix_bg_color").toString("#ffffff"));
    t.blockTextSize    = o.value("block_text_size").toInt(11);
    t.blockTextColor   = QColor(o.value("block_text_color").toString("#000000"));
    t.blockBgColor     = QColor(o.value("block_bg_color").toString("#ffffff"));
    return t;
}

QJsonObject writeTheme(const ThemeData &t)
{
    QJsonObject o;
    o["program_text_size"]  = t.programTextSize;
    o["program_text_color"] = t.programTextColor.name();
    o["program_bg_color"]   = t.programBgColor.name();
    o["matrix_bg_color"]    = t.matrixBgColor.name();
    o["block_text_size"]    = t.blockTextSize;
    o["block_text_color"]   = t.blockTextColor.name();
    o["block_bg_color"]     = t.blockBgColor.name();
    return o;
}

} // namespace

Theme &Theme::instance()
{
    static Theme s;
    return s;
}

void Theme::load()
{
    QJsonObject root;
    const auto path = APP_DIR + THEMES_REL;
    if (QFile::exists(path))
    {
        root = Loader::load(path);
    }
    else
    {
        root = Loader::load(QRC_THEMES);
        Loader::save(root, path);
    }

    m_currentTheme = root.value("current_theme").toString("light");

    m_themes.clear();
    const auto themesObj = root.value("themes").toObject();
    for (auto it = themesObj.begin(); it != themesObj.end(); ++it)
    {
        m_themes.insert(it.key(), readTheme(it.key(), it.value().toObject()));
    }

    if (m_themes.contains(m_currentTheme) == false)
    {
        m_currentTheme = m_themes.isEmpty() ? QString() : m_themes.firstKey();
    }
}

bool Theme::save()
{
    QJsonObject root;
    root["current_theme"] = m_currentTheme;

    QJsonObject themesObj;
    for (auto it = m_themes.begin(); it != m_themes.end(); ++it)
    {
        themesObj[it.key()] = writeTheme(it.value());
    }
    root["themes"] = themesObj;

    return Loader::save(root, APP_DIR + THEMES_REL);
}

void Theme::setCurrent(const QString &name)
{
    if (m_themes.contains(name))
    {
        m_currentTheme = name;
    }
}

ThemeData Theme::current() const
{
    return m_themes.value(m_currentTheme);
}

ThemeData Theme::get(const QString &name) const
{
    return m_themes.value(name);
}

void Theme::set(const QString &name, const ThemeData &data)
{
    auto t = data;
    t.name = name;
    m_themes.insert(name, t);
}