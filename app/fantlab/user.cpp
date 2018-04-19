#include "user.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>

User::User(int id, QNetworkAccessManager* m, QObject *parent): Request(m, parent), userId(id) {
}

QNetworkRequest* User::getRequest() const {
    return new QNetworkRequest(QUrl(Request::apiUrl + "/user/" + QString::number(userId)));
}
