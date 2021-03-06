#include <iterator>
#include <algorithm>

#include "cothinkermodel.h"
#include "user.h"
#include "misc.h"
#include "fs.h"
#include <QColor>
#include <QNetworkAccessManager>
#include <QFile>


CoThinkerModel::CoThinkerModel(QNetworkAccessManager* m, QObject *parent) :
    QAbstractListModel(parent), nam(m), useGenreFilterAndWorkDetails(false)
{
    sfDummy.title = tr("Skipped");
    sfDummy.description = tr("Skipped");
    sfDummy.workId = 0;
    sfDummy.name = tr("Skipped");
    sfDummy.genres.append(1);
}

int CoThinkerModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return co_thinkers.size();
}

int CoThinkerModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return CTM_COLCOUNT;
}

QVariant CoThinkerModel::data(const QModelIndex& index, int role) const {
    QVariant result;

    if (!index.isValid())
        return result;

    switch(role) {
    case Qt::DisplayRole:  {
        switch(index.column()) {
            case CTM_TYPE:  return at(index).myRecord?tr("My"):tr("Co Thinker");
            case CTM_URL:   return at(index).url;
            case CTM_NAME:  return at(index).name;
            case CTM_MARKS_PAIR:  return at(index).myRecord?tr("N/A"):QString::number(at(index).pairs);
            case CTM_SIMILARITY:  return at(index).myRecord?tr("N/A"):QString::number(at(index).similarity);
            case CTM_LOGIN: return at(index).login;
            case CTM_CLASS: return at(index).className;
            case CTM_MARK_COUNT: return at(index).markCount;
            case CTM_MESSAGE_COUNT: return at(index).messageCount;
            case CTM_RESPONSE_COUNT: return at(index).responseCount;
            case CTM_TICKETS_COUNT: return at(index).ticketsCount;
            case CTM_TOPIC_COUNT: return at(index).topicCount;
            case CTM_FAILCOUNT: return at(index).failCount;
            case CTM_STATUS: return at(index).status;
            case CTM_PROCESSED_MARKS: return at(index).processedMarks;
            default:
            break;
            }
        }
        break;
    case SortRole: {
        switch(index.column()) {
            case CTM_TYPE:      return at(index).myRecord?0:1;
            case CTM_URL:       return at(index).url;
            case CTM_NAME:      return at(index).name;
            case CTM_MARKS_PAIR:return at(index).pairs;
            case CTM_SIMILARITY:return at(index).similarity;
            case CTM_LOGIN: return at(index).login;
            case CTM_CLASS: return at(index).className;
            case CTM_MARK_COUNT: return at(index).markCount;
            case CTM_MESSAGE_COUNT: return at(index).messageCount;
            case CTM_RESPONSE_COUNT: return at(index).responseCount;
            case CTM_TICKETS_COUNT: return at(index).ticketsCount;
            case CTM_TOPIC_COUNT: return at(index).topicCount;
            case CTM_FAILCOUNT: return at(index).failCount;
            case CTM_STATUS: return at(index).status;
            case CTM_PROCESSED_MARKS: return at(index) .processedMarks;
            default:
            break;
            }
        }
        break;
    case Qt::ForegroundRole: {
            if (at(index).myRecord) return QColor("orange");
            if (at(index).similarity > 0) return QColor("green");
            if (at(index).similarity < 0) return QColor("red");
        }
        break;
    case Qt::TextAlignmentRole: {
        switch(index.column()) {
            case CTM_URL:
            case CTM_NAME:
            return Qt::AlignLeft;
        default:
            return Qt::AlignRight;
        }
    }
    default:
        break;
    }

    return QVariant();
}

QVariant CoThinkerModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal)
        return QAbstractListModel::headerData(section, orientation, role);

    if (role == Qt::TextAlignmentRole) {
        switch(section) {
        case CTM_URL:
        case CTM_NAME:
            return Qt::AlignLeft;
        default:
            return Qt::AlignRight;
        }
    }

    if (role == Qt::DisplayRole) {
        switch(section) {
            case CTM_TYPE:      return tr("Type");
            case CTM_URL:       return tr("Url");
            case CTM_NAME:      return tr("Name");
            case CTM_MARKS_PAIR:return tr("Pairs");
            case CTM_SIMILARITY:return tr("Similarity");
            case CTM_LOGIN: return tr("Login");
            case CTM_CLASS: return tr("Class");
            case CTM_MARK_COUNT: return tr("Marks");
            case CTM_MESSAGE_COUNT: return tr("Messages");
            case CTM_RESPONSE_COUNT: return tr("Responses");
            case CTM_TICKETS_COUNT: return tr("Tickets");
            case CTM_TOPIC_COUNT: return tr("Topics");
            case CTM_FAILCOUNT: return tr("Fail count");
            case CTM_STATUS: return tr("Status");
            case CTM_PROCESSED_MARKS: return tr("Processed marks");
            default:
                Q_ASSERT(false);
                break;
        }
    }

    return QVariant();
}

void CoThinkerModel::populate(const QList<QStringList>& data) {
    minSim = 1.0f;
    co_thinkers.reserve(data.size());
    int pos = 0;
    foreach(const QStringList& ct, data) {
        Q_ASSERT(ct.size() == 4);
        emit beginInsertRows(QModelIndex(), co_thinkers.size(), co_thinkers.size());
        co_thinkers.push_back(new User( pos++ == 0
                                        , ct.at(0)
                                        , ct.at(1)
                                        , ct.at(2).toInt()
                                        , ct.at(3).toDouble()
                                        , Misc::url2UserId(ct.at(0))
                                        , co_thinkers.size()
                                        , this
                                        , this));
        if (co_thinkers.back()->similarity > 0 && minSim > co_thinkers.back()->similarity) minSim = co_thinkers.back()->similarity;
        // register all users to save the same order in storage as in model list
        markStorage.addUser(co_thinkers.back()->userId);
        endInsertRows();
    }
}

const User& CoThinkerModel::at(const QModelIndex& index) const {
    Q_ASSERT(index.row() < rowCount());
    return *co_thinkers.at(index.row());
}

void CoThinkerModel::start(int minSim, int maxMarks) {
    updateIndex = 0;
    activeRequests = 0;
    totalCount = 0;
    errorCount = 0;
    executeRequests = true;

    for(User* u: co_thinkers) {
        if (acceptUser(u->myRecord, u->similarity, u->markCount, minSim, maxMarks)) u->prepareMarksRequests();
    }

    updateData(-1);
}

void CoThinkerModel::stop() {
    executeRequests = false;
}

void CoThinkerModel::updateData(int pos) {

    if (pos != -1) {
        Q_ASSERT(pos < rowCount());
        ++totalCount;
        errorCount += (co_thinkers.at(pos)->errorCode != 0)?1:0;
        emit dataChanged(index(pos, CTM_LOGIN), index(pos, CTM_PROCESSED_MARKS));
        emit networkRequestsUpdate(totalCount, errorCount);
    }

    while(requestSlots() > 0) {
        int before = requestSlots();

        foreach(User* au, active_users) {
            au->requestData();
        }

        // load new data here
        while(requestSlots() > 0 && updateIndex < co_thinkers.size()) {
            co_thinkers[updateIndex]->status = tr("Requested");
            emit dataChanged(index(updateIndex, CTM_STATUS), index(updateIndex, CTM_STATUS));
            active_users.append(co_thinkers[updateIndex]);
            active_users.back()->requestData();
            ++updateIndex;
        }

        if (before == requestSlots()) break;
    }

    if (activeRequests == 0) {
        // data request finished
        emit networkRequestsFinish(totalCount, errorCount);
    }
}

void CoThinkerModel::addWork(int workId, const WorkInfo& wi) {
    workDict.insert(workId, wi);
}

bool CoThinkerModel::hasWork(int workId) const {
    return workDict.contains(workId);
}

const WorkInfo& CoThinkerModel::getWork(int workId) const {
    static WorkInfo w;
    if (useGenreFilterAndWorkDetails) {
        QMap<int, WorkInfo>::const_iterator itr = workDict.find(workId);
        if (itr != workDict.end()) return itr.value();
        return w;
    }

    return sfDummy;
}

bool CoThinkerModel::takeRequestSlot() {
    if (requestSlots() > 0) {
        ++activeRequests;
        Q_ASSERT(activeRequests <= maxSimultaneousRequests);
        return true;
    }

    return false;
}

void CoThinkerModel::releaseRequestSlot() {
    --activeRequests;
    Q_ASSERT(activeRequests >= 0);
}

void CoThinkerModel::deactivateUser(User* user) {
    // deactivation can executed many times after all pending requests were started, but some requests still in progress
    bool res = active_users.removeOne(user);
    Q_UNUSED(res);
}

QList<User*> CoThinkerModel::getSimilarUsers(double minBorder) const {
    QList<User*> tmp;
    std::copy_if(co_thinkers.begin()
                 , co_thinkers.end()
                 , std::back_inserter(tmp)
                 , [minBorder](const User* u) -> bool { return u->similarity >= minBorder; });
    return std::move(tmp);
}

QList<bool> CoThinkerModel::getActiveUsers(int minSim, int maxMark) const {
    QList<bool> res;
    for(const User* u: co_thinkers) {
        res.append(acceptUser(u->myRecord, u->similarity, u->markCount, minSim, maxMark));
    }

    return res;
}

bool CoThinkerModel::load(const QString& filename) {
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream ds(&file);
        ds >> workDict;
        file.close();
        qDebug() << "load works from " << filename;
        return true;
    }

    return false;
}

void CoThinkerModel::save() const {
    QFile file(Utils::Fs::cacheLocation() + "/works.dat");

    if (file.open(QIODevice::WriteOnly)) {
        QDataStream ds(&file);
        ds << workDict;
        file.close();
    }
}

void CoThinkerModel::injectSelfId(int id) {
    if (co_thinkers.isEmpty() && id > 0) return;
    co_thinkers[0]->userId = id;
    co_thinkers[0]->url = QString("https://fantlab.ru/user%1").arg(id);
    qDebug() << "new id " << id;
    emit dataChanged(index(0, 0), index(0, CTM_URL));
}


