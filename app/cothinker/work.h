#ifndef WORK_H
#define WORK_H

#include <QObject>

QT_BEGIN_NAMESPACE
class Request;
class CoThinkerModel;
QT_END_NAMESPACE

class Work : public QObject {
    Q_OBJECT
private:
    Request* request;
    CoThinkerModel* model;
    int workId;
    void finishRequest();
public:
    explicit Work(CoThinkerModel* mod, int work, QObject *parent = 0);
    void requestData();
signals:

public slots:

private slots:
    void processResponse(const QJsonDocument&);
    void jsonError(int);
    void networkError(int);
};

#endif // WORK_H
