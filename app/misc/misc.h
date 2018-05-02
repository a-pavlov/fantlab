#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QPair>
#include <QList>

class QJsonDocument;

struct WorkInfo {
    QString title;
    QString description;
    QString name;
    QList<int> genres;
};

class Misc {
public:
    static int url2UserId(const QString&);
    static QPair<int, QString> octaveCost(const QString&);
    static bool isIteration(const QString&);
    static QString octaveLambda(const QString&);
    static QString octaveMinCost(const QString&);
    static bool isLambdaFinished(const QString&);
    static WorkInfo getWorkInfo(const QJsonDocument&);
};

#endif // UTILS_H
