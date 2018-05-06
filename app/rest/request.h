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
class QNetworkRequest;
QT_END_NAMESPACE

class Request : public QObject {
    Q_OBJECT
protected:
    int param;
public:
    static QString apiUrl;
    explicit Request(int p, QObject *parent = 0);
    virtual ~Request();
    void start(QNetworkAccessManager*);
    virtual QNetworkRequest getRequest() const = 0;
signals:
    void finished(int, const QJsonDocument&);
    void networkError(int, int);
    void jsonError(int, int);
private slots:
    void endRequest(QNetworkReply*);
    void sslErrors(QNetworkReply * reply, const QList<QSslError> & errors);
    void handleReplyError(QNetworkReply::NetworkError error);
    void handleSslErrors(QList<QSslError> errors);
};

#endif // REQUEST_H
