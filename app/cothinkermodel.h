#ifndef COTHINKERMODEL_H
#define COTHINKERMODEL_H

#include <QObject>
#include <QAbstractListModel>


struct CoThinker {
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
    int markCount;
    int messageCount;
    int responseCount;
    int ticketsCount;
    int topicCount;
    int failCount;

    QString login;
    QString className;
    OperStatus status;

    CoThinker(const QString& u, const QString& n, int p, double s):
        url(u)
        , name(n)
        , pairs(p)
        , similarity(s)
        , markCount(0)
        , messageCount(0)
        , responseCount(0)
        , ticketsCount(0)
        , topicCount(0)
        , failCount(0)
        , status(OS_NONE) {}

    static QString status2Str(OperStatus os);
};

class QNetworkAccessManager;
class User;

class CoThinkerModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        SortRole = Qt::UserRole + 1
    };

    enum Columns { CTM_URL
                   , CTM_NAME
                   , CTM_MARKS_PAIR
                   , CTM_SIMILARITY
                   , CTM_LOGIN
                   , CTM_CLASS
                   , CTM_MARK_COUNT
                   , CTM_MESSAGE_COUNT
                   , CTM_RESPONSE_COUNT
                   , CTM_TICKETS_COUNT
                   , CTM_TOPIC_COUNT
                   , CTM_FAILCOUNT
                   , CTM_STATUS
                   , CTM_COLCOUNT};

    explicit CoThinkerModel(QObject *parent = 0);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void populate(const QList<QStringList>&);
    void start(QNetworkAccessManager*);
    void updateData(User* u);
private:
    int updateIndex;
    QList<User*> pendingRequests;
    QList<CoThinker> co_thinkers;
    QNetworkAccessManager* nam;
    const CoThinker& at(const QModelIndex&) const;
signals:

private slots:

public slots:
};

#endif // COTHINKERMODEL_H
