#include "user.h"
#include "cothinkermodel.h"
#include "request.h"

#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>

User::User(const QString& u
           , const QString& nm
           , int pr
           , double sim
           , int id
           , int pos
           , CoThinkerModel* mod
           , QObject *parent): QObject(parent)
    , url(u)
    , name(nm)
    , pairs(pr)
    , similarity(sim)
    , userId(id)
    , position(pos)
    , markCount(0)
    , messageCount(0)
    , responseCount(0)
    , ticketsCount(0)
    , topicCount(0)
    , failCount(0)
    , errorCode(0)
    , model(mod) {
}

/*
User::User(const User& u) {
    url = u.url;
    name = u.name;
    pairs = u.pairs;
    similarity = u.similarity;
    userId = u.userId;
    model = u.model;
    manager = u.manager;
    login = u.login;
    className = u.className;
    markCount = u.markCount;
    messageCount = u.messageCount;
    responseCount = u.responseCount;
    ticketsCount = u.ticketsCount;
    topicCount = u.topicCount;
    failCount = u.failCount;
    status = u.status;
}
*/

User::~User() {
    qDebug() << "removed user " << getPosition();
}

void User::requestData() {
    Request* request = new Request(Request::apiUrl + "/user/" + QString::number(userId));
    connect(request, SIGNAL(finished(QJsonDocument)), this, SLOT(processResponse(QJsonDocument)));
    connect(request, SIGNAL(jsonError(int)), this, SLOT(jsonError(int)));
    connect(request, SIGNAL(networkError(int)), this, SLOT(networkError(int)));
    request->start(model->getNetworkManager());
}

void User::jsonError(int ec) {
    errorCode = ec;
    status = tr("Json error %1").arg(ec);
    model->updateData(position);
}

void User::networkError(int ec) {
    errorCode = ec;
    status = tr("Network error %1").arg(ec);
    model->updateData(position);
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
    status = tr("Finished");
    model->updateData(position);
}
