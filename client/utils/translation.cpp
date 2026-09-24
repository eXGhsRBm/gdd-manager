#include "translation.h"

#include "settings.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

Translation::~Translation()
{
    if (m_translator)
    {
        delete m_translator;
        m_translator = nullptr;
    }
}

Translation &Translation::instance()
{
    static Translation s;
    return s;
}

bool Translation::setLanguage(const QString &uiPath)
{
    if (m_translator == nullptr)
    {
        m_translator = new Translator;
    }

    if (m_translator->load(uiPath) == false)
    {
        return false;
    }

    QFileInfo fi(uiPath);
    auto errDir = fi.absolutePath();
    errDir.replace(QStringLiteral("/ui"), QStringLiteral("/errors"));
    const auto errPath = errDir + QLatin1Char('/') + fi.fileName();

    m_translator->loadErrors(errPath);

    QCoreApplication::removeTranslator(m_translator);
    QCoreApplication::installTranslator(m_translator);
    return true;
}

bool Translation::setProject(const QString &filePath)
{
    if (m_translator)
    {
        return m_translator->loadProject(filePath);
    }
    else
    {
        return false;
    }
}

void Translation::resetProject()
{
    if (m_translator)
    {
        m_translator->clearProject();
    }
}

QString Translation::doc(const QString &key) const
{
    return m_translator
               ? m_translator->doc(key)
               : key;
}

QString Translation::toc(const QString &docId,
                         const QString &secKey) const
{
    return m_translator
               ? m_translator->toc(docId, secKey)
               : secKey;
}

QString Translation::error(const QString &key) const
{
    return m_translator
               ? m_translator->error(key)
               : key;
}
