#include "work.h"
#include "request.h"
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
    Request* request = new Request(Request::apiUrl + "/work/" + QString::number(workId) + "/extended");
    connect(request, SIGNAL(finished(QJsonDocument)), this, SLOT(processResponse(QJsonDocument)));
    connect(request, SIGNAL(jsonError(int)), this, SLOT(jsonError(int)));
    connect(request, SIGNAL(networkError(int)), this, SLOT(networkError(int)));
    request->start(model->getNetworkManager());
}

void Work::jsonError(int ec) {
    Q_UNUSED(ec);
}

void Work::networkError(int ec) {
    Q_UNUSED(ec);
}

void Work::processResponse(const QJsonDocument& jd) {
    wi = Misc::getWorkInfo(jd);
    model->addWork(workId, wi);
}
