#include "recommendmodel.h"

#include <QFile>
#include <QTextStream>
#include <QColor>

RecommendModel::RecommendModel(QObject* parent/* = 0*/): QAbstractListModel(parent) {
}

int RecommendModel::rowCount(const QModelIndex& parent/* = QModelIndex()*/) const {
    Q_UNUSED(parent);
    return items.size();
}

int RecommendModel::columnCount(const QModelIndex& parent/* = QModelIndex()*/) const {
    Q_UNUSED(parent);
    return RM_COLCOUNT;
}

QVariant RecommendModel::data(const QModelIndex& index, int role) const {
    QVariant result;

    if (!index.isValid()) return result;

    switch(role) {
        case Qt::DisplayRole:  {
            switch(index.column()) {
                case RM_OWN_MARK:  return at(index).ownMark==0?tr("N/A"):QString::number(at(index).ownMark);
                case RM_MARK:  return at(index).mark;
                case RM_WORK:  return at(index).work;
                default:
                break;
                }
            }
            break;
        case SortRole: {
            switch(index.column()) {
                case RM_OWN_MARK:   return at(index).ownMark;
                case RM_MARK:       return at(index).mark;
                case RM_WORK:       return at(index).work;
                default:
                break;
                }
            }
            break;
        case Qt::ForegroundRole: return QColor("green");
        case Qt::TextAlignmentRole: return Qt::AlignRight;
        default:
            break;
    }

    return QVariant();
}

QVariant RecommendModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal) return QAbstractListModel::headerData(section, orientation, role);
    if (role == Qt::TextAlignmentRole) return Qt::AlignRight;

    if (role == Qt::DisplayRole) {
        switch(section) {
            case RM_OWN_MARK:   return tr("Own mark");
            case RM_MARK:       return tr("Predict");
            case RM_WORK:       return tr("Work");
            default:
                Q_ASSERT(false);
                break;
        }
    }

    return QVariant();
}

void RecommendModel::populate(const QString& fileName) {
    beginResetModel();
    items.clear();
    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly)) {
       QTextStream in(&inputFile);
       while (!in.atEnd()) {
           QString line = in.readLine();
          QVector<QStringRef> elems =  line.splitRef(",");
          if (elems.size() == 3) {
              items.append(RecommendItem());
              items.back().ownMark = elems.at(0).toFloat();
              items.back().mark = elems.at(1).toFloat();
              items.back().work = elems.at(2).toString();
          }
       }
       inputFile.close();
    }
    endResetModel();
}

const RecommendItem& RecommendModel::at(const QModelIndex& index) const {
    Q_ASSERT(index.row() < rowCount());
    return items.at(index.row());
}
