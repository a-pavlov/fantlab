#include "work.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>

Work::Work(QNetworkAccessManager* m, QObject *parent)
    : QObject(parent), manager(m) {
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(endRequest(QNetworkReply*)));
    connect(manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
}

void Work::sendRequest() {
    QNetworkRequest* rq = new QNetworkRequest(QUrl("https://api.fantlab.ru/work/7067/extended"));
    rq->setRawHeader("Content-Type", "application/json");
    rq->setOriginatingObject(this);
    QNetworkReply* reply = manager->get(*rq);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(handleReplyError(QNetworkReply::NetworkError)));

    connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
                this, SLOT(handleSslErrors(QList<QSslError>)));
}

void Work::endRequest(QNetworkReply* reply) {
    if (reply->request().originatingObject() != this) {
        // That's not the request sent by the object -> stop the method here !
        qDebug() << "not our request";
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << reply->error() << reply->errorString();
        //emit replyError(reply, reply->error(), reply->errorString());
    } else {
        qDebug() << reply->readAll();
    }

    qDebug() << "got our requst";
}

void Work::sslErrors(QNetworkReply * reply, const QList<QSslError> & errors) {
    qDebug() << "sll errors, ignore";
    reply->ignoreSslErrors();
}

void Work::handleReplyError(QNetworkReply::NetworkError error) {
    qDebug() << Q_FUNC_INFO;
    qDebug() << "Error" << error;
}

void Work::handleSslErrors(QList<QSslError> errors) {
    qDebug() << Q_FUNC_INFO;
    qDebug() << errors;
}
