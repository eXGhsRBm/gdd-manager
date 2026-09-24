#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QHash>
#include <QTranslator>

class Translator : public QTranslator
{
    Q_OBJECT

public:
    explicit Translator(QObject *parent = nullptr);

    bool load(const QString &filePath);

    QString translate(const char *context,
                      const char *sourceText,
                      const char *disambiguation = nullptr,
                      int n = -1) const override;

    bool isEmpty() const override;

    bool loadProject(const QString &docPath);
    void clearProject();

    QString doc(const QString &key) const;
    QString toc(const QString &docId,
                const QString &secKey) const;

    bool loadErrors(const QString &path);
    QString error(const QString &key) const;

private:
    QHash<QString, QString> m_map;
    QHash<QString, QString> m_doc;
    QHash<QString, QString> m_toc;
    QHash<QString, QString> m_err;
};

#endif