#include "utils.h"
#include <QRegExp>
#include <QStringList>


int Utils::url2UserId(const QString& url) {
    QRegExp rx("\\D*/user(\\d+)$");
    rx.indexIn(url);
    QStringList res = rx.capturedTexts();
    return res.size()==2?res.at(1).toInt():0;
}
