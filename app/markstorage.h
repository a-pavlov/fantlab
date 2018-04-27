#ifndef MARKSTORAGE_H
#define MARKSTORAGE_H

#include <QMap>
#include <QList>
#include <QStringList>

class MarkStorage {
private:
    /**
     * @brief workIndexes
     * work id as key, position as value
     */
    QMap<int, int> workIndexes;
    /**
     * @brief markMatrix
     * MxN
     * M users count
     * N works count
     * 2x4
     *    w1 w2 w3 w4
     * u1 *  *  *  *
     * u2 *  *  *  *
     *
     */
    QList<QList<int>> markMatrix;
    /**
     * @brief userIndexes
     * user id as key, position as value
     */
    QMap<int, int> userIndexes;
public:
    MarkStorage();
    void addMark(int user, int work, int mark);

    const QMap<int, int>& getWorkIndexes() const {
        return workIndexes;
    }

    const QList<QList<int>>& getMarkMatrix() const {
        return markMatrix;
    }

    const QMap<int, int>& getUsers() const {
        return userIndexes;
    }

    int getTotalMarks() const {
        return markMatrix.isEmpty()?0:markMatrix.at(0).size();
    }

    QStringList getMarksByIndex(int i) {
        Q_ASSERT(i < getTotalMarks());
        QStringList res;
        foreach(const QList<int>& userMarks, markMatrix) {
            res << QString::number(userMarks.at(i));
        }

        return res;
    }

    bool saveData(const QString& worksFilename, const QString& marksFilename);
};

#endif // MARKSTORAGE_H
