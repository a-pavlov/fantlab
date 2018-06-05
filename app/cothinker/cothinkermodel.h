#ifndef COTHINKERMODEL_H
#define COTHINKERMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "user.h"
#include "misc.h"
#include "markstorage.h"

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
QT_END_NAMESPACE

class CoThinkerModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        SortRole = Qt::UserRole + 1
    };

    static bool acceptUser(bool self, double sim, int marks, int minSim, int maxMarks) {
        return self || (sim*100 >= minSim && marks <= maxMarks);
    }

    const static int maxSimultaneousRequests = 20;

    enum Columns {
                    CTM_TYPE
                   , CTM_URL
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
                   , CTM_PROCESSED_MARKS
                   , CTM_COLCOUNT};

    explicit CoThinkerModel(QNetworkAccessManager* man, QObject *parent = 0);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void populate(const QList<QStringList>&);
    void start(int minSim, int maxMarks);
    void stop();
    void updateData(int pos);
    QNetworkAccessManager* getNetworkManager() const { return nam; }
    void addWork(int workId, const WorkInfo& wi);
    bool hasWork(int workId) const;
    const WorkInfo& getWork(int workId) const;
    int requestSlots() const { return executeRequests?maxSimultaneousRequests - activeRequests:0; }
    int getActiveRequests() const { return activeRequests; }

    MarkStorage& getMarkStorage() {
        return markStorage;
    }

    const MarkStorage& getMarkStorage() const {
        return markStorage;
    }

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

    double getMinSim() const { return minSim; }
    QList<User*> getSimilarUsers(double minBorder) const;

    QList<bool> getActiveUsers(int minSim, int maxMark) const;

    void load();
    void save() const;
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
    MarkStorage markStorage;
    double minSim;
signals:
    void networkRequestsFinish(int totalCount, int errorsCount);
    void networkRequestsUpdate(int totalCount, int errorsCount);
private slots:

public slots:
};

#endif // COTHINKERMODEL_H
