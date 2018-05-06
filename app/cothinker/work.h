#ifndef WORK_H
#define WORK_H

#include <QObject>
#include "misc.h"

QT_BEGIN_NAMESPACE
class CoThinkerModel;
QT_END_NAMESPACE

class Work : public QObject {
    Q_OBJECT
private:
    CoThinkerModel* model;
    int workId;
    WorkInfo wi;
public:
    explicit Work(CoThinkerModel* mod, int work, QObject *parent = 0);
    void requestData();
signals:

private slots:
    void processResponse(int,const QJsonDocument&);
    void jsonError(int,int);
    void networkError(int,int);
};

#endif // WORK_H
