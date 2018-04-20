#ifndef MARKSTORAGE_H
#define MARKSTORAGE_H

#include <QMap>
#include <QList>

class MarkStorage {
private:
    QMap<int, int> workIndexes;
    QList<QList<int>> markMatrix;
    QList<int> users;
public:
    MarkStorage();
    void addMark(int user, int work, int mark);

    const QMap<int, int>& getWorkIndexes() const {
        return workIndexes;
    }

    const QList<QList<int>>& getMarkMatrix() const {
        return markMatrix;
    }

    const QList<int>& getUsers() const {
        return users;
    }

    int getTotalMarks() const {
        return markMatrix.isEmpty()?0:markMatrix.at(0).size();
    }

    QList<int> getMarksByIndex(int i) {
        Q_ASSERT(i < getTotalMarks());
        QList<int> res;
        foreach(const QList<int>& userMarks, markMatrix) {
            res.append(userMarks.at(i));
        }

        return res;
    }
};

#endif // MARKSTORAGE_H
