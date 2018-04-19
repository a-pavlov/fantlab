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
private:
    QNetworkAccessManager* manager;
public:
    static QString apiUrl;
    explicit Request(QNetworkAccessManager* m, QObject *parent = 0);
    void start();
    virtual QNetworkRequest* getRequest() const = 0;
    virtual void processResponse(const QJsonDocument&) = 0;
signals:

private slots:
    void endRequest(QNetworkReply* replay);
    void sslErrors(QNetworkReply * reply, const QList<QSslError> & errors);
    void handleReplyError(QNetworkReply::NetworkError error);
    void handleSslErrors(QList<QSslError> errors);
};

#endif // WORK_H
