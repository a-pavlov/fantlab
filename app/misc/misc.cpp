#include "misc.h"
#include <QRegExp>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

QDataStream &operator<<(QDataStream& out, const WorkInfo& wi) {
    out << wi.workId
        << wi.name
        << wi.title
        << wi.description
        << wi.genres;
    return out;
}

QDataStream &operator>>(QDataStream& in, WorkInfo& wi) {
    in >> wi.workId >> wi.name >> wi.title >> wi.description >> wi.genres;
    return in;
}

int Misc::url2UserId(const QString& url) {
    static QRegExp rx("\\D*/user(\\d+)$");
    rx.indexIn(url);
    QStringList res = rx.capturedTexts();
    return res.size()==2?res.at(1).toInt():0;
}

QPair<int, QString> Misc::octaveCost(const QString& input) {
    static QRegExp rx("\\s*(\\d+)\\s+\\|\\s+Cost:\\s*([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)");
    rx.indexIn(input);
    QStringList res = rx.capturedTexts();
    return (res.size() == 4)?qMakePair(res.at(1).toInt(), res.at(2)):qMakePair(0,QString());
}

bool Misc::isIteration(const QString& input) {
    static QRegExp rx("^Iteration\\s*\\r?\\n?");
    return rx.exactMatch(input);
}

QString Misc::octaveLambda(const QString& input) {
    static QRegExp rx("cur_lambda:([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)");
    rx.indexIn(input);
    QStringList res = rx.capturedTexts();
    return (res.size() == 3)?res.at(1):QString();
}

QString Misc::octaveMinCost(const QString& input) {
    static QRegExp rx("min_cost:([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)");
    rx.indexIn(input);
    QStringList res = rx.capturedTexts();
    return (res.size() == 3)?res.at(1):QString();
}

bool Misc::isLambdaFinished(const QString& input) {
    static QRegExp rx("^fin_lambda\\s*\\r?\\n?");
    return rx.exactMatch(input);
}

WorkInfo Misc::getWorkInfo(const QJsonDocument& doc) {
    WorkInfo wi;
    QJsonObject o = doc.object();
    wi.workId = o["work_id"].toString().trimmed().toInt();
    wi.title = o["title"].toString();
    wi.name = o["work_name"].toString();
    wi.description = o["work_description"].toString();
    QJsonArray classificatory = o["classificatory"].toArray();

    foreach(const QJsonValue& cv, classificatory) {
        if (cv.toObject()["genre_group_id"].toInt() == 1) {
            QJsonArray ga = cv.toObject()["genre"].toArray();
            foreach(const QJsonValue& gv, ga) {
                wi.genres.append(gv.toObject()["genre_id"].toInt());
            }
            break;
        }
    }

    return wi;
}

bool Misc::isSF(const WorkInfo& wi) {
    return wi.genres.contains(1);
}
