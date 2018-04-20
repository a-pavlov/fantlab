#include "cothinkermodel.h"
#include "user.h"
#include "utils.h"
#include <QColor>
#include <QNetworkAccessManager>

QString CoThinker::status2Str(CoThinker::OperStatus os) {
    const static QString ss[]  = {
        QString("None"),
        QString("Requested"),
        QString("Finished"),
        QString("Failed")
    };

    return ss[os];
}

CoThinkerModel::CoThinkerModel(QObject *parent) :
    QAbstractListModel(parent), nam(NULL)
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
            case CTM_MARKS: return at(index).marks;
            case CTM_FAILCOUNT: return at(index).failCount;
            case CTM_STATUS: return CoThinker::status2Str(at(index).status);
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
            case CTM_MARKS: return at(index).marks;
            case CTM_FAILCOUNT: return at(index).failCount;
            case CTM_STATUS: return CoThinker::status2Str(at(index).status);
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
            case CTM_MARKS: return tr("Marks");
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
    foreach(const QStringList& ct, data) {
        Q_ASSERT(ct.size() == 4);
        emit beginInsertRows(QModelIndex(), co_thinkers.size(), co_thinkers.size());
        co_thinkers.push_back(CoThinker(ct.at(0)
                                        , ct.at(1)
                                        , ct.at(2).toInt()
                                        , ct.at(3).toDouble()));
        endInsertRows();
    }
}

const CoThinker& CoThinkerModel::at(const QModelIndex& index) const {
    Q_ASSERT(index.row() < rowCount());
    return co_thinkers.at(index.row());
}

void CoThinkerModel::start(QNetworkAccessManager* m) {
    Q_ASSERT(nam == NULL);
    nam = m;
    updateIndex = 0;
    updateData(NULL);
}


void CoThinkerModel::updateData(User* u) {

    // remove request
    if (u != NULL) {
        co_thinkers[u->getPosition()].className = u->getClassName();
        co_thinkers[u->getPosition()].login = u->getLogin();
        co_thinkers[u->getPosition()].marks = u->getMarks();
        co_thinkers[u->getPosition()].status = CoThinker::OS_FINISHED;
        emit dataChanged(index(u->getPosition(), CTM_LOGIN), index(u->getPosition(), CTM_STATUS));
        bool b = pendingRequests.removeOne(u);
        Q_ASSERT(b);
        u->deleteLater();
    }

    // load new data here
    if (pendingRequests.size() < 5) {
        while(pendingRequests.size() < 15 && updateIndex < co_thinkers.size()) {
            co_thinkers[updateIndex].status = CoThinker::OS_PENDING;
            emit dataChanged(index(updateIndex, CTM_STATUS), index(updateIndex, CTM_STATUS));
            pendingRequests.append(new User(Utils::url2UserId(co_thinkers[updateIndex].url), updateIndex, this, nam, this));
            pendingRequests.last()->start();
            ++updateIndex;
        }
    }
}
