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
private:
    int updateIndex;
    int pendingRequests;
    int totalCount;
    int errorCount;
    bool executeRequests;
    QList<User*> co_thinkers;
    QNetworkAccessManager* nam;
    const User& at(const QModelIndex&) const;
    QMap<int, WorkInfo> workDict;
signals:
    void dataRefreshed(int totalCount, int errorsCount);
private slots:

public slots:
};

#endif // COTHINKERMODEL_H
