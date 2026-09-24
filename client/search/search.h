#ifndef SEARCH_H
#define SEARCH_H

#include <QList>
#include <QString>

struct SearchMatch {
    QString path;
    QString name;
    int     line     = 0;
    QString preview;
    bool    isFolder = false;
    bool    isFile   = false;
};

struct SearchOptions {
    bool folders       = false;
    bool files         = false;
    bool content       = false;
    bool caseSensitive = false;
};

class Search
{
public:
    static QList<SearchMatch> find(const QString &rootDir,
                                   const QString &phrase,
                                   const SearchOptions &opts);

private:
    static QList<SearchMatch> searchNames(const QString &root,
                                          const QString &phrase,
                                          const SearchOptions &opts);

    static QList<SearchMatch> searchContent(const QString &root,
                                            const QString &phrase,
                                            const SearchOptions &opts);

    static QList<SearchMatch> scanFile(const QString &absPath,
                                       const QString &phrase,
                                       bool caseSensitive);
};

#endif