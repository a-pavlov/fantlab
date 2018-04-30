#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QPair>

class Misc {
public:
    static int url2UserId(const QString&);
    static QPair<int, QString> octaveCost(const QString&);
    static bool isIteration(const QString&);
    static QString octaveLambda(const QString&);
    static QString octaveMinCost(const QString&);
    static bool isLambdaFinished(const QString&);
};

#endif // UTILS_H
