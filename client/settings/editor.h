#ifndef EDITOR_H
#define EDITOR_H

#include <QMap>
#include <QString>
#include <QStringList>

struct EditorData {
    QString name;
    QString path;
    QString args;
};

class Editor
{
public:
    static Editor &instance();

    void load();
    bool save();

    EditorData get(const QString &name) const;
    void       set(const QString &name,
             const EditorData &data);

    QString  defaultEditor() const
    { return m_default; }

    void     setDefault(const QString &name);

    QStringList names() const
    { return m_editors.keys(); }

    void clear()
    { m_editors.clear(); }

private:
    Editor() = default;
    Editor(const Editor &) = delete;
    Editor &operator=(const Editor &) = delete;

    QString m_default;
    QMap<QString, EditorData> m_editors;
};

#endif // EDITOR_H