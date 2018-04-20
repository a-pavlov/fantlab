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
    int marks;
    int position;
    CoThinkerModel* model;
    QString login;
    QString className;
public:
    User(int id, int pos, CoThinkerModel* mod, QNetworkAccessManager* m, QObject *parent = 0);
    ~User();
    QNetworkRequest* getRequest() const;
    void processResponse(const QJsonDocument&);

    const QString& getLogin() const {
        return login;
    }

    const QString& getClassName() const {
        return className;
    }

    int getMarks() const {
        return marks;
    }

    int getPosition() const {
        return position;
    }
};

#endif // USER_H
