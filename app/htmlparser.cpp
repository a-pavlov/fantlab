#include "htmlparser.h"

#include <QDebug>

HtmlParser::HtmlParser() : currentIndex(0) {
    stack.append(QSharedPointer<Indexer>(new Controller("tbody")));
    stack.append(QSharedPointer<Indexer>(new Consumer(96)));
    stack.append(QSharedPointer<Indexer>(new Controller("href")));
    stack.append(QSharedPointer<Indexer>(new Consumer(1)));
    stack.append(QSharedPointer<Indexer>(new Collector()));
    stack.append(QSharedPointer<Indexer>(new Consumer(2)));
    stack.append(QSharedPointer<Indexer>(new Collector()));
    stack.append(QSharedPointer<Indexer>(new Consumer(142)));
    stack.append(QSharedPointer<Indexer>(new Controller(">")));
    stack.append(QSharedPointer<Indexer>(new Collector()));
    stack.append(QSharedPointer<Indexer>(new Consumer(31)));
    stack.append(QSharedPointer<Indexer>(new Controller("b")));
    stack.append(QSharedPointer<Indexer>(new Controller(">")));
    stack.append(QSharedPointer<Indexer>(new Collector()));
    pos = 0;
}

void HtmlParser::exec(const char* data, size_t len) {
    /*
    if (QString::fromUtf8(data, len) == "tbody") {
        pos = 0;
    } else if (QString::fromUtf8(data, len) == "href") {    
    } else {
        pos++;
    }
    */

    currentIndex = stack[currentIndex]->process(data, len, currentIndex, stack.size());

    if (currentIndex >= stack.size()) {
        // the end has been reached
        currentIndex = 0;
        QStringList list;
        foreach(QSharedPointer<Indexer> i, stack) {
            if (!i->getData().isNull()) list.append(i->getData());
        }
        result.append(list);
    }
}

HtmlParser::Indexer::~Indexer() {}

int HtmlParser::HtmlParser::Indexer::process(const char* data, size_t len, int currentIndex, int totalIndexes) {
    return processImpl(data, len, currentIndex, totalIndexes);
}

QString HtmlParser::Indexer::getData() const {
    return QString();
}

HtmlParser::Consumer::Consumer(int c) : count(c), counter(c) {}

int HtmlParser::Consumer::processImpl(const char* data, size_t len, int currentIndex, int totalIndexes) {
    Q_UNUSED(data);
    Q_UNUSED(len);
    Q_UNUSED(totalIndexes);    

    --counter;

    if (counter == 0) {
        counter = count;
        return currentIndex + 1;
    }

    return currentIndex;
}


HtmlParser::Controller::Controller(QString t): tmpl(t) {

}

int HtmlParser::Controller::processImpl(const char* data, size_t len, int currentIndex, int totalIndexes) {
    Q_UNUSED(totalIndexes);

    if (QString::fromUtf8(data, static_cast<int>(len)) == tmpl) {
        //qDebug() << "found " << tmpl << " set index to " << currentIndex + 1;
        return currentIndex + 1;
    }

    return 0;
}

int HtmlParser::Collector::processImpl(const char* data, size_t len, int currentIndex, int totalIndexes) {
    accum = QString::fromUtf8(data, len);
    return currentIndex + 1;
}
