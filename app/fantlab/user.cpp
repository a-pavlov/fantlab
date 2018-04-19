#include "user.h"
#include "cothinkermodel.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>

User::User(int id, int pos, CoThinkerModel* mod, QNetworkAccessManager* m, QObject *parent): Request(m, parent)
  , userId(id)
  , position(pos)
  , model(mod) {
}

User::~User() {
    qDebug() << "removed user " << getPosition();
}

QNetworkRequest* User::getRequest() const {
    return new QNetworkRequest(QUrl(Request::apiUrl + "/user/" + QString::number(userId)));
}

void User::processResponse(const QJsonDocument& jd) {
    qDebug() << "is obj " << jd.isObject();
    QJsonObject o = jd.object();
    login = o["login"].toString();
    className = o["class_name"].toString();
    marks = o["markcount"].toString().trimmed().toInt();
    qDebug() << "login " << login << " class " << className << " marks " << marks;
    if (model != NULL) {
        model->updateData(this);
    }
}
