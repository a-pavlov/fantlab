#include "workrequest.h"
#include <QNetworkRequest>
#include <QUrl>

WorkRequest::WorkRequest(int param, QObject* parent /* = 0*/)
    : Request(param, parent) {
}

QNetworkRequest WorkRequest::getRequest() const {
    return QNetworkRequest(QUrl(Request::apiUrl + "/work/" + QString::number(param) + "/extended"));
}
