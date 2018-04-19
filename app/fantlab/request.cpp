#include "request.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>

QString Request::apiUrl = QString("https://api.fantlab.ru");

Request::Request(QNetworkAccessManager* m, QObject *parent)
    : QObject(parent), manager(m) {
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(endRequest(QNetworkReply*)));
    connect(manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
}

void Request::start() {
    QNetworkRequest* request = getRequest();
    request->setRawHeader("Content-Type", "application/json");
    request->setOriginatingObject(this);
    QNetworkReply* reply = manager->get(*request);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(handleReplyError(QNetworkReply::NetworkError)));

    connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
                this, SLOT(handleSslErrors(QList<QSslError>)));
}

void Request::endRequest(QNetworkReply* reply) {
    if (reply->request().originatingObject() != this) {
        // That's not the request sent by the object -> stop the method here !
        qDebug() << "not our request";
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << reply->error() << reply->errorString();
        //emit replyError(reply, reply->error(), reply->errorString());
    } else {
        QJsonParseError error;
        QJsonDocument jd = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error == QJsonParseError::NoError) processResponse(jd);
    }

    qDebug() << "got our requst";
}

void Request::sslErrors(QNetworkReply * reply, const QList<QSslError> & errors) {
    qDebug() << "sll errors, ignore";
    reply->ignoreSslErrors();
}

void Request::handleReplyError(QNetworkReply::NetworkError error) {
    qDebug() << Q_FUNC_INFO;
    qDebug() << "Error" << error;
}

void Request::handleSslErrors(QList<QSslError> errors) {
    qDebug() << Q_FUNC_INFO;
    qDebug() << errors;
}
