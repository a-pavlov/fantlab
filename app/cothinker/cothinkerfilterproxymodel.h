#ifndef COTHINKERFILTERPROXYMODEL_H
#define COTHINKERFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class CoThinkerFilterProxyModel : public QSortFilterProxyModel {
public:
    CoThinkerFilterProxyModel(int minSim, int maxMark, QObject* parent = 0);
    virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const;
    void setFilterParameter(int minSim, int maxMark);
protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;

private:
    int minSimilarity;
    int maxMarksCount;
};

#endif // COTHINKERFILTERPROXYMODEL_H
