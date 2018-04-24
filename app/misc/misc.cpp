#include "misc.h"
#include <QRegExp>
#include <QStringList>


int Utils::url2UserId(const QString& url) {
    static QRegExp rx("\\D*/user(\\d+)$");
    rx.indexIn(url);
    QStringList res = rx.capturedTexts();
    return res.size()==2?res.at(1).toInt():0;
}

QPair<int, QString> Utils::octaveCost(const QString& input) {
    static QRegExp rx("\\s*(\\d+)\\s+\\|\\s+Cost:\\s*([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)");
    rx.indexIn(input);
    QStringList res = rx.capturedTexts();
    return (res.size() == 4)?qMakePair(res.at(1).toInt(), res.at(2)):qMakePair(0,QString());
}

bool Utils::isIteration(const QString& input) {
    static QRegExp rx("^Iteration\\s*\\r?\\n?");
    return rx.exactMatch(input);
}
