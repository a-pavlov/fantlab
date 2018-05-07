#include "markrequest.h"

MarkRequest::MarkRequest(int user, int pr, QObject* parent /* = 0*/)
    : Request(pr, parent), userId(user) {
}

QNetworkRequest MarkRequest::getRequest() const {
    return QNetworkRequest(QUrl(Request::apiUrl + "/user/" + QString::number(userId) + "/marks?page=" + QString::number(param)));
}
