#include "cothinkermodel.h"
#include <QColor>

CoThinkerModel::CoThinkerModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int CoThinkerModel::rowCount(const QModelIndex& parent) const {
    return co_thinkers.size();
}

int CoThinkerModel::columnCount(const QModelIndex& parent) const {
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
            case CTM_MARKS_PAIR:  return at(index).marks;
            case CTM_SIMILARITY:  return at(index).similarity;
            default:
            break;
            }
        }
        break;
    case SortRole: {
        switch(index.column()) {
            case CTM_URL:       return at(index).url;
            case CTM_NAME:      return at(index).name;
            case CTM_MARKS_PAIR:return at(index).marks;
            case CTM_SIMILARITY:return at(index).similarity;
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
