#ifndef THEME_H
#define THEME_H

#include <QColor>
#include <QMap>
#include <QString>
#include <QStringList>

struct ThemeData {
    QString name;

    int     programTextSize    = 10;
    QColor  programTextColor   = QColor("#000000");
    QColor  programBorderColor = QColor("#000000");
    QColor  programBgColor     = QColor("#ffffff");

    QColor  matrixBgColor      = QColor("#ffffff");

    int     blockTextSize      = 11;
    QColor  blockTextColor     = QColor("#000000");
    QColor  blockBgColor       = QColor("#ffffff");
};

class Theme
{
public:
    static Theme &instance();

    void load();
    bool save();

    ThemeData current() const;
    QString   currentName() const
    { return m_currentTheme; }

    void setCurrent(const QString &name);

    ThemeData get(const QString &name) const;
    void      set(const QString &name,
             const ThemeData &data);

    QStringList names() const
    { return m_themes.keys(); }

private:
    Theme() = default;
    Theme(const Theme &) = delete;
    Theme &operator=(const Theme &) = delete;

    QString m_currentTheme;
    QMap<QString, ThemeData> m_themes;
};

#endif // THEME_H