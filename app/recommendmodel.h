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
                   , RM_COLCOUNT};

    RecommendModel(QObject* parent = 0);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void populate(const QString& fileName);
private:
    const RecommendItem& at(const QModelIndex&) const;
    QList<RecommendItem> items;
};

#endif // RECOMMENDMODEL_H
