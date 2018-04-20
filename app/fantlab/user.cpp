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
    , model(mod)
    , markCount(0)
    , messageCount(0)
    , responseCount(0)
    , ticketsCount(0)
    , topicCount(0) {
}

User::~User() {
    qDebug() << "removed user " << getPosition();
}

QNetworkRequest* User::getRequest() const {
    return new QNetworkRequest(QUrl(Request::apiUrl + "/user/" + QString::number(userId)));
}

void User::processResponse(const QJsonDocument& jd) {
    QJsonObject o = jd.object();
    login = o["login"].toString();
    className = o["class_name"].toString();
    markCount = o["markcount"].toString().trimmed().toInt();
    messageCount = o["messagecount"].toString().trimmed().toInt();
    responseCount = o["responsecount"].toString().trimmed().toInt();
    ticketsCount = o["tickets_count"].toString().trimmed().toInt();
    topicCount = o["topiccount"].toString().trimmed().toInt();

    if (model != NULL) {
        model->updateData(this);
    }
}
