#ifndef COTHINKERMODEL_H
#define COTHINKERMODEL_H

#include <QObject>
#include <QAbstractListModel>

struct CoThinker {
    QString url;
    QString name;
    int marks;
    double similarity;
    CoThinker(const QString& u, const QString& n, int m, double s):
        url(u)
        , name(n)
        , marks(m)
        , similarity(s) {}
};

class CoThinkerModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        SortRole = Qt::UserRole + 1
    };

    enum Columns { CTM_URL, CTM_NAME, CTM_MARKS_PAIR, CTM_SIMILARITY, CTM_COLCOUNT};

    explicit CoThinkerModel(QObject *parent = 0);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void populate(const QList<QStringList>&);
private:
    QList<CoThinker> co_thinkers;
    const CoThinker& at(const QModelIndex&) const;
signals:

public slots:
};

#endif // COTHINKERMODEL_H
