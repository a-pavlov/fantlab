#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <functional>

QT_BEGIN_NAMESPACE
class CoThinkerModel;
class Request;
QT_END_NAMESPACE

class User : public QObject {
    Q_OBJECT
public:
    QString url;
    QString name;
    int pairs;
    double similarity;
    int userId;
    int position;
    QString login;
    QString className;
    int markCount;
    int messageCount;
    int responseCount;
    int ticketsCount;
    int topicCount;
    int failCount;
    QString status;
    int errorCode;
    CoThinkerModel* model;
    QMap<int, int> pendingWorkMarks;

    User(const QString& u
         , const QString& nm
         , int pr
         , double sim
         , int id
         , int pos
         , CoThinkerModel* mod
         , QObject *parent = 0);

    ~User();
    void requestData();

    int getPosition() const { return position; }
private:
    QList<std::function<void()>>    pendingOperations;
    void finishRequst();
private slots:
    void processDetailsResponse(int,const QJsonDocument&);
    void processMarksResponse(int,const QJsonDocument&);
    void processWorkResponse(int,const QJsonDocument&);
    void jsonError(int,int);
    void networkError(int,int);
};

#endif // USER_H
