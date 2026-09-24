#ifndef TRANSLATION_H
#define TRANSLATION_H

#include "translator.h"

class Translation
{
public:
    static Translation &instance();
    ~Translation();

    bool setLanguage(const QString &filePath);

    bool setProject(const QString &filePath);
    void resetProject();

    QString doc(const QString &key) const;
    QString toc(const QString &docId,
                const QString &secKey) const;

    QString error(const QString &key) const;

private:
    Translation() = default;
    Translation(const Translation &) = delete;
    Translation &operator=(const Translation &) = delete;

    Translator *m_translator = nullptr;
};

#define TRANSLATE_DOC Translation::instance().doc
#define TRANSLATE_TOC Translation::instance().toc
#define TRANSLATE_ERR Translation::instance().error

#endif // TRANSLATION_H
