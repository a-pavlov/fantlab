#include "cothinkermodel.h"
#include "user.h"
#include "utils.h"
#include <QColor>
#include <QNetworkAccessManager>


CoThinkerModel::CoThinkerModel(QNetworkAccessManager* m, QObject *parent) :
    QAbstractListModel(parent), nam(m)
{
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
            case CTM_URL:   return at(index).url;
            case CTM_NAME:  return at(index).name;
            case CTM_MARKS_PAIR:  return at(index).pairs;
            case CTM_SIMILARITY:  return at(index).similarity;
            case CTM_LOGIN: return at(index).login;
            case CTM_CLASS: return at(index).className;
            case CTM_MARK_COUNT: return at(index).markCount;
            case CTM_MESSAGE_COUNT: return at(index).messageCount;
            case CTM_RESPONSE_COUNT: return at(index).responseCount;
            case CTM_TICKETS_COUNT: return at(index).ticketsCount;
            case CTM_TOPIC_COUNT: return at(index).topicCount;
            case CTM_FAILCOUNT: return at(index).failCount;
            case CTM_STATUS: return at(index).status;
            default:
            break;
            }
        }
        break;
    case SortRole: {
        switch(index.column()) {
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
            default:
            break;
            }
        }
        break;
    case Qt::ForegroundRole: {
            if (at(index).similarity > 0) return QColor("green");
            if (at(index).similarity < 0) return QColor("orange");
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
            default:
                Q_ASSERT(false);
                break;
        }
    }

    return QVariant();
}

void CoThinkerModel::populate(const QList<QStringList>& data) {
    co_thinkers.reserve(data.size());
    foreach(const QStringList& ct, data) {
        Q_ASSERT(ct.size() == 4);
        emit beginInsertRows(QModelIndex(), co_thinkers.size(), co_thinkers.size());
        co_thinkers.push_back(new User(ct.at(0)
                                        , ct.at(1)
                                        , ct.at(2).toInt()
                                        , ct.at(3).toDouble()
                                        , Utils::url2UserId(ct.at(0))
                                        , co_thinkers.size()
                                        , this
                                        , this));
        endInsertRows();
    }
}

const User& CoThinkerModel::at(const QModelIndex& index) const {
    Q_ASSERT(index.row() < rowCount());
    return *co_thinkers.at(index.row());
}

void CoThinkerModel::start() {
    updateIndex = 0;
    pendingRequests = 0;
    totalCount = 0;
    errorCount = 0;
    executeRequests = true;
    updateData(-1);
}

void CoThinkerModel::stop() {
    executeRequests = false;
}

void CoThinkerModel::updateData(int pos) {
    if (pos != -1) {
        Q_ASSERT(pos < rowCount());
        // position finished, update model
        --pendingRequests;
        ++totalCount;
        errorCount += (co_thinkers.at(pos)->errorCode != 0)?1:0;
        emit dataChanged(index(pos, CTM_LOGIN), index(pos, CTM_STATUS));
    }

    // load new data here
    if (executeRequests && pendingRequests < 5) {
        while(pendingRequests < 15 && updateIndex < co_thinkers.size()) {
            co_thinkers[updateIndex]->status = tr("Requested");
            emit dataChanged(index(updateIndex, CTM_STATUS), index(updateIndex, CTM_STATUS));
            co_thinkers[updateIndex]->requestData();
            ++updateIndex;
            ++pendingRequests;
        }
    }

    if (pendingRequests == 0) {
        // data request finished
        emit dataRefreshed(totalCount, errorCount);
    }
}
