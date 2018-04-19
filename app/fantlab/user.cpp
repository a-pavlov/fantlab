#include "user.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>

User::User(int id, QNetworkAccessManager* m, QObject *parent): Request(m, parent), userId(id) {
}

QNetworkRequest* User::getRequest() const {
    return new QNetworkRequest(QUrl(Request::apiUrl + "/user/" + QString::number(userId)));
}

void User::processResponse(const QJsonDocument& jd) {
    qDebug() << "is obj " << jd.isObject();
    QJsonObject o = jd.object();
    QString login = o["login"].toString();
    QString className = o["class_name"].toString();
    int marks = o["markcount"].toInt();
    qDebug() << "login " << login << " class " << className << " marks " << marks;
}
