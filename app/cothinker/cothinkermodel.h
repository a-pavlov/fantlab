#ifndef COTHINKERMODEL_H
#define COTHINKERMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "user.h"
#include "misc.h"

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
QT_END_NAMESPACE

class CoThinkerModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        SortRole = Qt::UserRole + 1
    };

    const static int maxSimultaneousRequests = 20;

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

    explicit CoThinkerModel(QNetworkAccessManager* man, QObject *parent = 0);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void populate(const QList<QStringList>&);
    void start();
    void stop();
    void updateData(int pos);
    QNetworkAccessManager* getNetworkManager() const { return nam; }
    void addWork(int workId, const WorkInfo& wi);
    bool hasWork(int workId) const;
    int requestSlots() const { return executeRequests?maxSimultaneousRequests - activeRequests:0; }

    /**
     * @brief takeRequestSlot
     * @return true if new request slot was allocated
     */
    bool takeRequestSlot();

    /**
     * @brief releaseRequestSlot return slot to origin
     */
    void releaseRequestSlot();

    /**
     * @brief deactivateUser remove user from active users list when user completed requests
     * @param user pointer to object
     */
    void deactivateUser(User* user);
private:
    int updateIndex;
    int activeRequests;
    int totalCount;
    int errorCount;
    bool executeRequests;
    QList<User*> co_thinkers;
    QList<User*> active_users;
    QNetworkAccessManager* nam;
    const User& at(const QModelIndex&) const;
    QMap<int, WorkInfo> workDict;
signals:
    void dataRefreshed(int totalCount, int errorsCount);
private slots:

public slots:
};

#endif // COTHINKERMODEL_H
