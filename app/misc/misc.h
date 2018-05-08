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
    inline bool isNull() const {
        return name.isNull() || genres.isEmpty();
    }
};

namespace Misc2 {
    template <typename A, typename B>
    inline A divCeil(A a, B b) { return A((a + b - 1)/b); }

}

class Misc {
public:
    static int url2UserId(const QString&);
    static QPair<int, QString> octaveCost(const QString&);
    static bool isIteration(const QString&);
    static QString octaveLambda(const QString&);
    static QString octaveMinCost(const QString&);
    static bool isLambdaFinished(const QString&);
    static WorkInfo getWorkInfo(const QJsonDocument&);
    static bool isSF(const WorkInfo&);
};

#endif // UTILS_H
