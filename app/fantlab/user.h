#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>
#include "request.h"

class QNetworkAccessManager;
class QNetworkRequest;
class CoThinkerModel;

class User : public Request {
    Q_OBJECT    

private:
    int userId;
    int position;
    CoThinkerModel* model;
public:
    QString login;
    QString className;
    int markCount;
    int messageCount;
    int responseCount;
    int ticketsCount;
    int topicCount;

    User(int id, int pos, CoThinkerModel* mod, QNetworkAccessManager* m, QObject *parent = 0);
    ~User();
    QNetworkRequest* getRequest() const;
    void processResponse(const QJsonDocument&);    

    int getPosition() const {
        return position;
    }
};

#endif // USER_H
