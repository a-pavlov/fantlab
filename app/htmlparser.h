#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QStringList>

class HtmlParser: public QObject
{
    Q_OBJECT
private:
    class Indexer {
    public:
        virtual ~Indexer();
        virtual int processImpl(const char* data, size_t len, int currentIndex, int totalIndexes) = 0;
        int process(const char* data, size_t len, int currentIndex, int totalIndexes);
        virtual QString getData() const;
    };

    class Consumer : public Indexer {
    private:
        int count;
        int counter;
    public:
        Consumer(int);
        virtual int processImpl(const char* data, size_t len, int currentIndex, int totalIndexes);
    };

    class Controller : public Indexer {
    private:
        QString tmpl;
    public:
        Controller(QString);
        virtual int processImpl(const char* data, size_t len, int currentIndex, int totalIndexes);
    };

    class Collector : public Indexer {
    private:
        QString accum;
    public:
        virtual int processImpl(const char* data, size_t len, int currentIndex, int totalIndexes);
        virtual QString getData() const { return accum; }
    };

    QList<QSharedPointer<Indexer>> stack;
    int currentIndex;
    int pos;
    QList<QStringList> result;
public:
    HtmlParser();
    void exec(const char* data, size_t len);

    const QList<QStringList>& getResults() const {
        return result;
    }

    QList<QStringList>& getResults() {
        return result;
    }
};

#endif // HTMLPARSER_H
