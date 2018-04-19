#ifndef USER_H
#define USER_H

#include <QObject>
#include "request.h"

class QNetworkAccessManager;
class QNetworkRequest;

class User : public Request {
    Q_OBJECT
private:
    int userId;
public:
    User(int, QNetworkAccessManager* m, QObject *parent = 0);
    QNetworkRequest* getRequest() const;
    void processResponse(const QJsonDocument&);
};

#endif // USER_H
