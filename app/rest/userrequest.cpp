#include "userrequest.h"
#include <QNetworkRequest>
#include <QUrl>

UserRequest::UserRequest(int param, QObject* parent /* = 0*/)
    : Request(param, parent) {
}

QNetworkRequest UserRequest::getRequest() const {
    return QNetworkRequest(QUrl(Request::apiUrl + "/user/" + QString::number(param)));
}
