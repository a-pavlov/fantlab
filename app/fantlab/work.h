#ifndef WORK_H
#define WORK_H

#include <QObject>
#include <QList>
#include <QSslError>
#include <QNetworkReply>

class QNetworkAccessManager;

class Work : public QObject
{
    Q_OBJECT
private:
    QNetworkAccessManager* manager;
public:
    explicit Work(QNetworkAccessManager* m, QObject *parent = 0);

    void sendRequest();
signals:

private slots:
    void endRequest(QNetworkReply* replay);
    void sslErrors(QNetworkReply * reply, const QList<QSslError> & errors);
    void handleReplyError(QNetworkReply::NetworkError error);
    void handleSslErrors(QList<QSslError> errors);
};

#endif // WORK_H
