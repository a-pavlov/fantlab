#include "work.h"
#include "workrequest.h"
#include "cothinkermodel.h"

#include <QNetworkAccessManager>
#include <QUrl>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>

Work::Work(CoThinkerModel* mod, int work, QObject *parent) :
    QObject(parent)
    , model(mod)
    , workId(work) {

}

void Work::requestData() {
    Request* request = new WorkRequest(workId);
    connect(request, SIGNAL(finished(int,QJsonDocument)), this, SLOT(processResponse(int,QJsonDocument)));
    connect(request, SIGNAL(jsonError(int,int)), this, SLOT(jsonError(int,int)));
    connect(request, SIGNAL(networkError(int,int)), this, SLOT(networkError(int,int)));
    request->start(model->getNetworkManager());
}

void Work::jsonError(int param, int ec) {
    Q_UNUSED(param);
    Q_UNUSED(ec);
}

void Work::networkError(int param, int ec) {
    Q_UNUSED(param);
    Q_UNUSED(ec);
}

void Work::processResponse(int param, const QJsonDocument& jd) {
    Q_UNUSED(param);
    wi = Misc::getWorkInfo(jd);
    model->addWork(workId, wi);
}
