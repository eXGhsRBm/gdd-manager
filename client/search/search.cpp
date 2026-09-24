#include "search.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

static const qint64 MAX_FILE_SIZE = 10 * 1024 * 1024;   // 10 MB

static bool isTextFile(const QString &path)
{
    static const QStringList exts = {
        QStringLiteral("md"),   QStringLiteral("json"),
        QStringLiteral("txt"),  QStringLiteral("xml"),
        QStringLiteral("csv"),  QStringLiteral("yml"),
        QStringLiteral("yaml")
    };
    return exts.contains(QFileInfo(path).suffix().toLower());
}

QList<SearchMatch> Search::find(const QString &rootDir,
                                const QString &phrase,
                                const SearchOptions &opts)
{
    QList<SearchMatch> result;
    if (phrase.isEmpty() || rootDir.isEmpty() || !QDir(rootDir).exists())
    {
        return result;
    }

    if (opts.folders || opts.files)
    {
        result += searchNames(rootDir, phrase, opts);
    }

    if (opts.content)
    {
        result += searchContent(rootDir, phrase, opts);
    }
    return result;
}

QList<SearchMatch> Search::searchNames(const QString &root,
                                       const QString &phrase,
                                       const SearchOptions &opts)
{
    QList<SearchMatch> result;
    const auto flags = opts.caseSensitive
                           ? Qt::CaseSensitive
                           : Qt::CaseInsensitive;

    QDirIterator it(root, QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        const auto path = it.next();
        const QFileInfo fi(path);

        if (opts.folders && fi.isDir() && fi.fileName().contains(phrase, flags))
        {
            SearchMatch m;
            m.path = fi.absoluteFilePath();
            m.name = fi.fileName();
            m.isFolder = true;
            result << m;
        }

        if (opts.files && fi.isFile() && fi.fileName().contains(phrase, flags))
        {
            SearchMatch m;
            m.path = fi.absoluteFilePath();
            m.name = fi.fileName();
            m.isFile = true;
            result << m;
        }
    }
    return result;
}

static const int MAX_RESULTS = 5000;

QList<SearchMatch> Search::searchContent(const QString &root,
                                         const QString &phrase,
                                         const SearchOptions &opts)
{
    QList<SearchMatch> result;

    QDirIterator it(root, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        if (result.size() >= MAX_RESULTS)
        {
            break;
        }

        const auto path = it.next();
        const QFileInfo fi(path);
        if (fi.size() > MAX_FILE_SIZE)
        {
            continue;
        }

        if (isTextFile(path) == false)
        {
            continue;
        }

        result += scanFile(path, phrase, opts.caseSensitive);
    }
    return result;
}

QList<SearchMatch> Search::scanFile(const QString &absPath,
                                    const QString &phrase,
                                    bool caseSensitive)
{
    QList<SearchMatch> result;

    QFile f(absPath);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text) == false)
    {
        return result;
    }

    QTextStream in(&f);
    in.setEncoding(QStringConverter::Utf8);

    const auto flags = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

    int lineNo = 0;
    while (in.atEnd() == false)
    {
        const auto line = in.readLine();
        ++lineNo;

        if (line.contains(phrase, flags))
        {
            SearchMatch m;
            m.path    = absPath;
            m.name    = QFileInfo(absPath).fileName();
            m.line    = lineNo;
            m.preview = line.trimmed();
            if (m.preview.length() > 150)
            {
                m.preview = m.preview.left(147) + QStringLiteral("...");
            }
            m.isFile = true;
            result << m;
        }
    }
    return result;
}