#include "work.h"
#include "request.h"
#include "cothinkermodel.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>

Work::Work(CoThinkerModel* mod, int work, QObject *parent) :
    QObject(parent)
    , request(NULL)
    , model(mod)
    , workId(work) {

}

void Work::requestData() {
    Q_ASSERT(request == NULL);
    request = new Request();
    connect(request, SIGNAL(finished(QJsonDocument)), this, SLOT(processResponse(QJsonDocument)));
    connect(request, SIGNAL(jsonError(int)), this, SLOT(jsonError(int)));
    connect(request, SIGNAL(networkError(int)), this, SLOT(networkError(int)));
    request->start(new QNetworkRequest(QUrl(Request::apiUrl + "/work/" + QString::number(workId) + "/extended")), model->getNetworkManager());
}

void Work::finishRequest() {
    Q_ASSERT(request != NULL);
    disconnect(request, 0, 0, 0);
    request->deleteLater();
    request = NULL;
}

void Work::jsonError(int ec) {
    Q_UNUSED(ec);
    finishRequest();
}

void Work::networkError(int ec) {
    Q_UNUSED(ec);
    finishRequest();
}

void Work::processResponse(const QJsonDocument& jd) {
    wi = Misc::getWorkInfo(jd);
    model->addWork(workId, wi);
    finishRequest();
}
