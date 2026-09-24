#include "scanner.h"
#include "loader.h"
#include "project.h"
#include "settings.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include <QSet>
#include <QTextStream>
#include <algorithm>

namespace {

int findLineByValue(const QString &jsonPath,
                    const QString &value)
{
    if (value.isEmpty())
    {
        return 0;
    }

    QFile f(jsonPath);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text) == false)
    {
        return 0;
    }

    QTextStream in(&f);
    in.setEncoding(QStringConverter::Utf8);

    const auto needle = QStringLiteral("\"%1\"").arg(value);
    int lineNo = 0;
    while (in.atEnd() == false)
    {
        const auto line = in.readLine();
        ++lineNo;
        if (line.contains(needle))
        {
            return lineNo;
        }
    }
    return 0;
}

int findLineOfFilesInDoc(const QString &jsonPath,
                         const QString &folder)
{
    QFile f(jsonPath);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text) == false)
    {
        return 0;
    }

    QTextStream in(&f);
    in.setEncoding(QStringConverter::Utf8);

    QVector<QString> lines;
    while (in.atEnd() == false)
    {
        lines.append(in.readLine());
    }

    const auto folderNeedle = QStringLiteral("\"folder\": \"%1\"").arg(folder);
    for (auto i = 0; i < lines.size(); ++i)
    {
        if (lines[i].contains(folderNeedle) == false)
        {
            continue;
        }

        for (auto j = i; j >= 0 && j > i - 20; --j)
        {
            if (lines[j].trimmed().startsWith(QStringLiteral("\"files\"")))
            {
                return j + 1;
            }
        }
        break;
    }
    return 0;
}

int findLineByFolder(const QString &jsonPath,
                     const QString &folder)
{
    if (folder.isEmpty())
    {
        return 0;
    }

    QFile f(jsonPath);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text) == false)
    {
        return 0;
    }

    QTextStream in(&f);
    in.setEncoding(QStringConverter::Utf8);

    const auto needle = QStringLiteral("\"folder\": \"%1\"").arg(folder);
    int lineNo = 0;
    while (in.atEnd() == false)
    {
        const auto line = in.readLine();
        ++lineNo;
        if (line.contains(needle, Qt::CaseInsensitive))
        {
            return lineNo;
        }
    }
    return 0;
}

int findLineById(const QString &jsonPath,
                 const QString &id)
{
    QFile f(jsonPath);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text) == false)
    {
        return 0;
    }

    QTextStream in(&f);
    in.setEncoding(QStringConverter::Utf8);

    const auto needle = id.isEmpty()
                            ? QStringLiteral("\"id\": \"\"")
                            : QStringLiteral("\"id\": \"%1\"").arg(id);

    int lineNo = 0;
    while (in.atEnd() == false)
    {
        const auto line = in.readLine();
        ++lineNo;
        if (line.contains(needle, Qt::CaseInsensitive))
        {
            return lineNo;
        }
    }
    return 0;
}

void addIssue(QList<ScanIssue> &out,
              const QString &errorId,
              const QString &details,
              const QString &path,
              int line = 0)
{
    ScanIssue i;
    i.errorId = errorId;
    i.details = details;
    i.path    = path;
    i.line    = line;
    out << i;
}

void checkProject(const ProjectData &proj,
                  const QString &projectJsonPath,
                  QList<ScanIssue> &out)
{
    QSet<QString> docIds, catIds, stageIds;

    for (const auto &d : proj.documents)
    {
        const auto line = findLineById(projectJsonPath, d.id);
        if (d.id.isEmpty())
        {
            addIssue(out, "empty_doc_id", {}, projectJsonPath, line);
        }
        else if (docIds.contains(d.id))
        {
            addIssue(out, "duplicate_doc_id", d.id, projectJsonPath, line);
        }
        else
        {
            docIds.insert(d.id);
        }
    }

    for (const auto &c : proj.categories)
    {
        const auto line = findLineById(projectJsonPath, c.id);
        if (c.id.isEmpty())
        {
            addIssue(out, "empty_cat_id", {}, projectJsonPath, line);
        }
        else if (catIds.contains(c.id))
        {
            addIssue(out, "duplicate_cat_id", c.id, projectJsonPath, line);
        }
        else
        {
            catIds.insert(c.id);
        }
    }

    for (const auto &s : proj.stages)
    {
        const auto line = findLineById(projectJsonPath, s.id);
        if (s.id.isEmpty())
        {
            addIssue(out, "empty_stage_id", {}, projectJsonPath, line);
        }
        else if (stageIds.contains(s.id))
        {
            addIssue(out, "duplicate_stage_id", s.id, projectJsonPath, line);
        }
        else
        {
            stageIds.insert(s.id);
        }
    }

    for (const auto &d : proj.documents)
    {
        const auto line = findLineById(projectJsonPath, d.id);
        if (d.category.isEmpty() == false && catIds.contains(d.category) == false)
        {
            addIssue(out, "unknown_category", d.category, projectJsonPath, line);
        }

        if (d.stage.isEmpty() == false && stageIds.contains(d.stage) == false)
        {
            addIssue(out, "unknown_stage", d.stage, projectJsonPath, line);
        }
    }

    QSet<QString> tocByDoc;
    for (const auto &d : proj.documents)
    {
        if (d.tocPath.isEmpty())
        {
            continue;
        }

        const auto line = findLineByValue(projectJsonPath, QFileInfo(d.tocPath).fileName());

        if (tocByDoc.contains(d.id))
        {
            addIssue(out, "multiple_toc", d.id, projectJsonPath, line);
        }
        else
        {
            tocByDoc.insert(d.id);
        }

        if (QFile::exists(d.tocPath) == false)
        {
            addIssue(out, "missing_toc", QFileInfo(d.tocPath).fileName(), projectJsonPath, line);
        }
    }

    if (proj.language.isEmpty() == false && QFile::exists(proj.language) == false)
    {
        const auto line = findLineByValue(projectJsonPath, proj.language);
        addIssue(out, "missing_language", QFileInfo(proj.language).fileName(), projectJsonPath, line);
    }
}

void checkDocuments(const ProjectData &proj,
                    const QString &projectDir,
                    const QString &projectJsonPath,
                    QList<ScanIssue> &out)
{
    QSet<QString> knownPaths;
    for (const auto &d : proj.documents)
    {
        for (const auto &f : d.files)
        {
            const auto rel = d.folder + "/" + f;
            const auto abs = QDir::cleanPath(projectDir + "/" + rel);
            knownPaths.insert(abs);

            const auto fileLine = findLineByValue(projectJsonPath, f);

            if (QFile::exists(abs))
            {
                const QFileInfo fi(abs);
                if (fi.size() == 0)
                {
                    addIssue(out, "empty_document", f, projectJsonPath, fileLine);
                }
                continue;
            }

            const auto folderAbs = projectDir + "/" + d.folder;
            QDir folder(folderAbs);
            const auto mdFiles = folder.entryInfoList({"*.md"}, QDir::Files, QDir::Name);

            if (mdFiles.isEmpty())
            {
                addIssue(out, "document_file_missing", f, projectJsonPath, fileLine);
            }
            else
            {
                const auto actual = mdFiles.first().fileName();
                addIssue(out, "file_name_mismatch", QStringLiteral("%1 → %2").arg(f, actual), projectJsonPath, fileLine);
                knownPaths.insert(mdFiles.first().absoluteFilePath());
            }
        }
    }

    QDirIterator it(projectDir, {"*.md"}, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        const auto path = QDir::cleanPath(it.next());

        if (knownPaths.contains(path))
        {
            continue;
        }

        const auto rel = QDir(projectDir).relativeFilePath(path);
        if (rel.startsWith('.'))
        {
            continue;
        }

        const auto folder = QFileInfo(rel).path();
        const auto line   = (folder.isEmpty() || folder == ".")
                              ? 0
                              : findLineOfFilesInDoc(projectJsonPath, folder);

        addIssue(out, "file_not_in_project", QFileInfo(path).fileName(), projectJsonPath, line);
    }
}

void checkLinks(const QString &absPath, QList<ScanIssue> &out)
{
    QFile f(absPath);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text) == false)
    {
        addIssue(out, "no_read_access", QFileInfo(absPath).fileName(), absPath);
        return;
    }

    QTextStream in(&f);
    in.setEncoding(QStringConverter::Utf8);

    static const QRegularExpression reLink(R"(\[[^\]]*\]\(([^)]+)\))");
    const QFileInfo fi(absPath);
    const auto baseDir = fi.absoluteDir();

    int lineNo = 0;
    while (in.atEnd() == false)
    {
        const auto line = in.readLine();
        ++lineNo;

        auto mIt = reLink.globalMatch(line);
        while (mIt.hasNext())
        {
            const auto m = mIt.next();
            auto target = m.captured(1).trimmed();

            if (target.startsWith("http://") || target.startsWith("https://"))
            {
                continue;
            }
            if (target.startsWith("file://"))
            {
                continue;
            }

            const auto anchor = target.section('#', 1, 1);
            target = target.section('#', 0, 0);

            if (target.isEmpty())
            {
                continue;
            }

            const auto resolved = QDir::cleanPath(baseDir.absoluteFilePath(target));

            if (QFile::exists(resolved) == false)
            {
                addIssue(out, "broken_link", target, absPath, lineNo);
                continue;
            }

            if (anchor.isEmpty() == false)
            {
                QFile tf(resolved);
                if (tf.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    QTextStream tin(&tf);
                    tin.setEncoding(QStringConverter::Utf8);
                    bool found = false;

                    while (tin.atEnd() == false)
                    {
                        const QString l = tin.readLine();
                        if (l.startsWith('#') == false)
                        {
                            continue;
                        }

                        auto slug = l.mid(1).trimmed().toLower();
                        slug.replace(' ', '-');
                        if (slug == anchor.toLower())
                        {
                            found = true;
                            break;
                        }
                    }

                    if (found == false)
                    {
                        addIssue(out, "broken_anchor", target + "#" + anchor, absPath, lineNo);
                    }
                }
            }
        }
    }
}

void checkToc(const QString &tocPath, QList<ScanIssue> &out)
{
    if (QFile::exists(tocPath) == false)
    {
        return;
    }

    const auto root = Loader::load(tocPath);
    if (root.isEmpty())
    {
        addIssue(out, "invalid_json", {}, tocPath);
        return;
    }

    int idLine = 0;
    {
        QFile f(tocPath);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&f);
            int ln = 0;
            while (in.atEnd() == false)
            {
                ++ln;
                if (in.readLine().contains(QStringLiteral("\"id\"")))
                {
                    idLine = ln;
                    break;
                }
            }
        }
    }

    if (root.value("id").toString().isEmpty())
    {
        addIssue(out,
                 "toc_missing_id",
                 {},
                 tocPath,
                 idLine);
    }

    const auto sections = root.value("sections").toArray();
    QHash<QString, int> sectionLines;

    QFile f(tocPath);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&f);
        int ln = 0;
        while (in.atEnd() == false)
        {
            const auto l = in.readLine();
            ++ln;
            const auto trimmed = l.trimmed();
            if (trimmed.startsWith('"') && trimmed.endsWith(','))
            {
                const auto val = trimmed.mid(1, trimmed.length() - 3);
                if (sectionLines.contains(val) == false)
                {
                    sectionLines.insert(val, ln);
                }
            }
            else if (trimmed.startsWith('"') && trimmed.endsWith('"'))
            {
                const auto val = trimmed.mid(1, trimmed.length() - 2);
                if (sectionLines.contains(val) == false)
                {
                    sectionLines.insert(val, ln);
                }
            }
        }
    }


    QSet<QString> seenForDup;
    for (const auto &v : sections)
    {
        const auto s = v.toString();
        const auto line = sectionLines.value(s, 0);

        if (s.isEmpty())
        {
            QFile f(tocPath);
            int line = 0;
            if (f.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream in(&f);
                int ln = 0;
                while (!in.atEnd())
                {
                    ++ln;
                    if (in.readLine().trimmed() == QStringLiteral("\"\","))
                    {
                        line = ln;
                        break;
                    }
                }
            }
            addIssue(out, "toc_empty_section", {}, tocPath, line);
        }
        else if (seenForDup.contains(s))
        {
            addIssue(out, "toc_duplicate_section", s, tocPath, line);
        }
        else
        {
            seenForDup.insert(s);
        }
    }
}

} // namespace

QList<ScanIssue> Scanner::scan(const QString &projectDir,
                               const QString &projectJsonPath)
{
    QList<ScanIssue> out;

    const auto proj = Project::load(projectJsonPath);
    if (proj.name.isEmpty())
    {
        addIssue(out, "invalid_json", {}, projectJsonPath);
        return out;
    }

    checkProject(proj, projectJsonPath, out);
    checkDocuments(proj, projectDir, projectJsonPath, out);

    QDirIterator it(projectDir, {"*.md"}, QDir::Files, QDirIterator::Subdirectories);
    int mdCount = 0;
    while (it.hasNext())
    {
        ++mdCount;
        checkLinks(it.next(), out);
    }

    QSet<QString> checkedTocs;
    for (const auto &d : proj.documents)
    {
        if (d.tocPath.isEmpty())
        {
            continue;
        }

        if (checkedTocs.contains(d.tocPath))
        {
            continue;
        }

        checkedTocs.insert(d.tocPath);
        checkToc(d.tocPath, out);
    }

    std::sort(out.begin(), out.end(), [](const ScanIssue &a, const ScanIssue &b) {
                  const auto c = QString::compare(a.path, b.path, Qt::CaseInsensitive);
                  if (c != 0)
                  {
                      return c < 0;
                  }
                  return a.line < b.line;
              });

    return out;
}