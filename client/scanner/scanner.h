#ifndef SCANNER_H
#define SCANNER_H

#include <QList>
#include <QString>
#include <QStringList>

struct ScanIssue {
    QString errorId;
    QString details;
    QString path;
    int     line = 0;
};

class Scanner
{
public:
    static QList<ScanIssue> scan(const QString &projectDir,
                                 const QString &projectJsonPath);
};

#endif