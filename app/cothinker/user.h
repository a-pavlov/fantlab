#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>

class CoThinkerModel;
class Request;

class User : public QObject {
    Q_OBJECT    

private:
    Request* request;
public:
    enum OperStatus {
        OS_NONE = 0,
        OS_PENDING,
        OS_FINISHED,
        OS_ERROR
    };

    QString url;
    QString name;
    int pairs;
    double similarity;
    int userId;
    int position;
    CoThinkerModel* model;
    QString login;
    QString className;
    int markCount;
    int messageCount;
    int responseCount;
    int ticketsCount;
    int topicCount;
    int failCount;
    QString status;

    User(const QString& u
         , const QString& nm
         , int pr
         , double sim
         , int id
         , int pos
         , CoThinkerModel* mod
         , QObject *parent = 0);

    //User(const User&);

    ~User();
    void requestData();

    int getPosition() const { return position; }

private slots:
    void processResponse(const QJsonDocument&);
    void jsonError(int);
    void networkError(int);
};

#endif // USER_H
