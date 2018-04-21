#ifndef WORK_H
#define WORK_H

#include <QObject>
#include <QList>
#include <QSslError>
#include <QNetworkReply>
#include <QJsonDocument>

class QNetworkAccessManager;
class QNetworkRequest;

class Request : public QObject {
    Q_OBJECT    
public:
    static QString apiUrl;
    explicit Request(QObject *parent = 0);
    void start(QNetworkRequest*, QNetworkAccessManager*);
signals:
    void finished(const QJsonDocument&);
    void networkError(int);
    void jsonError(int);
private slots:
    void endRequest(QNetworkReply*);
    void sslErrors(QNetworkReply * reply, const QList<QSslError> & errors);
    void handleReplyError(QNetworkReply::NetworkError error);
    void handleSslErrors(QList<QSslError> errors);
};

#endif // WORK_H
