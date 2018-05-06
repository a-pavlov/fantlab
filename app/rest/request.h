#ifndef REQUEST_H
#define REQUEST_H

#include <QObject>
#include <QList>
#include <QSslError>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
QT_END_NAMESPACE

class Request : public QObject {
    Q_OBJECT
private:
    QNetworkRequest request;
public:
    static QString apiUrl;
    explicit Request(const QString& url, QObject *parent = 0);
    ~Request();
    void start(QNetworkAccessManager*);
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

#endif // REQUEST_H
