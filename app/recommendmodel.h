#ifndef RECOMMENDMODEL_H
#define RECOMMENDMODEL_H

#include <QObject>
#include <QList>
#include <QAbstractListModel>

struct RecommendItem {
    float ownMark;
    float mark;
    QString work;

    RecommendItem(): ownMark(0), mark(0) {}
};

QT_BEGIN_NAMESPACE
class CoThinkerModel;
QT_END_NAMESPACE

class RecommendModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        SortRole = Qt::UserRole + 1
    };

    enum Columns {
                    RM_OWN_MARK
                    , RM_MARK
                    , RM_WORK
                    , RM_TITLE
                    , RM_TYPE
                    , RM_TYPE_NAME
                    , RM_YEAR
                    , RM_DESCR
                    , RM_COLCOUNT};

    RecommendModel(const CoThinkerModel* cm, QObject* parent = 0);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void populate(const QString& fileName);
private:
    const RecommendItem& at(const QModelIndex&) const;
    QList<RecommendItem> items;
    const CoThinkerModel* co_thinkers;
};

#endif // RECOMMENDMODEL_H
