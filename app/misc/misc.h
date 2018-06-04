#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QPair>
#include <QList>
#include <QDataStream>

class QJsonDocument;

struct WorkInfo {
    qint32 workId;
    QString title;
    QString description;
    QString name;
    QString workType;
    qint32 workTypeId;
    QString workTypeName;
    qint32 workYear;
    QList<qint32> genres;
    inline bool isNull() const {
        return name.isNull() || genres.isEmpty();
    }
};

QDataStream &operator<<(QDataStream& out, const WorkInfo& wi);
QDataStream &operator>>(QDataStream& in, WorkInfo& wi);

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
