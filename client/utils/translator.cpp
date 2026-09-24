#include "translator.h"

#include "settings.h"
#include "loader.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

static QHash<QString, QString> readFlat(const QString &path)
{
    QHash<QString, QString> map;
    const auto root = Loader::load(path);
    for (auto it = root.begin(); it != root.end(); ++it)
    {
        if (it.value().isString())
        {
            map.insert(it.key(), it.value().toString());
        }
    }
    return map;
}

static QString makeKey(const QString &ctx,
                       const QString &src)
{
    return ctx + QChar(0x1F) + src;
}


Translator::Translator(QObject *parent)
    : QTranslator(parent)
{}

bool Translator::isEmpty() const
{
    return m_map.isEmpty();
}

bool Translator::load(const QString &filePath)
{
    QFile f(filePath);
    if (f.open(QIODevice::ReadOnly) == false)
    {
        return false;
    }

    const auto root = QJsonDocument::fromJson(f.readAll()).object();
    m_map.clear();

    for (auto ctxIt = root.begin(); ctxIt != root.end(); ++ctxIt)
    {
        const auto ctxObj = ctxIt.value().toObject();
        for (auto srcIt = ctxObj.begin(); srcIt != ctxObj.end(); ++srcIt)
        {
            m_map.insert(makeKey(ctxIt.key(), srcIt.key()), srcIt.value().toString());
        }
    }
    return !m_map.isEmpty();
}

bool Translator::loadProject(const QString &docPath)
{
    if (QFile::exists(docPath) == false)
    {
        return false;
    }

    const auto newDoc = readFlat(docPath);
    if (newDoc.isEmpty())
    {
        return false;
    }

    QFileInfo fi(docPath);
    auto dir = fi.absoluteDir();
    dir.cdUp();
    dir.cd("toc");
    const auto tocPath = dir.absoluteFilePath(fi.fileName());

    const auto newToc = readFlat(tocPath);
    if (newToc.isEmpty())
    {
        return false;
    }

    m_doc = newDoc;
    m_toc = newToc;
    return true;
}

void Translator::clearProject()
{
    m_doc.clear();
    m_toc.clear();
}

QString Translator::translate(const char *context,
                              const char *sourceText,
                              const char *disambiguation,
                              int n) const
{
    Q_UNUSED(disambiguation)
    Q_UNUSED(n)

    if (!sourceText)
    {
        return {};
    }

    const auto ctx = QString::fromUtf8(context ? context : "");
    const auto src = QString::fromUtf8(sourceText);

    auto it = m_map.constFind(makeKey(ctx, src));
    if (it != m_map.constEnd())
    {
        return it.value();
    }

    if (ctx != QStringLiteral("Common"))
    {
        it = m_map.constFind(makeKey(QStringLiteral("Common"), src));
        if (it != m_map.constEnd())
        {
            return it.value();
        }
    }

    return {};
}

QString Translator::doc(const QString &key) const
{
    return m_doc.value(key, key);
}

QString Translator::toc(const QString &docId,
                        const QString &secKey) const
{
    const auto full = docId + QLatin1Char('.') + secKey;
    return m_toc.value(full, secKey);
}

bool Translator::loadErrors(const QString &path)
{
    if (QFile::exists(path) == false)
    {
        return false;
    }

    const auto newErr = readFlat(path);
    if (newErr.isEmpty())
    {
        return false;
    }

    m_err = newErr;
    return true;
}

QString Translator::error(const QString &key) const
{
    return m_err.value(key, key);
}