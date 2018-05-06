#include "request.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>

QString Request::apiUrl = QString("https://api.fantlab.ru");

Request::Request(const QString& u, QObject *parent)
    : QObject(parent), request(QUrl(u)) {
    request.setRawHeader("Content-Type", "application/json");
    request.setOriginatingObject(this);
}

Request::~Request() {
    //qDebug() << "delete request " << request.url();
}

void Request::start(QNetworkAccessManager* manager) {
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(endRequest(QNetworkReply*)));
    connect(manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
    QNetworkReply* reply = manager->get(request);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(handleReplyError(QNetworkReply::NetworkError)));

    connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
                this, SLOT(handleSslErrors(QList<QSslError>)));
}

void Request::endRequest(QNetworkReply* reply) {
    if (reply->request().originatingObject() != this) {
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        emit networkError(static_cast<int>(reply->error()));
    } else {
        QJsonParseError error;
        QJsonDocument jd = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error == QJsonParseError::NoError) {
            emit finished(jd);
        } else {
            emit jsonError(static_cast<int>(error.error));
        }
    }

    disconnect(this, 0, 0, 0);
    deleteLater();
}

void Request::sslErrors(QNetworkReply * reply, const QList<QSslError> & errors) {
    Q_UNUSED(errors);
    reply->ignoreSslErrors();
}

void Request::handleReplyError(QNetworkReply::NetworkError error) {
    emit networkError(static_cast<int>(error));
    disconnect(this, 0, 0, 0);
    deleteLater();
}

void Request::handleSslErrors(QList<QSslError> errors) {
    qDebug() << Q_FUNC_INFO;
    qDebug() << errors;
}
