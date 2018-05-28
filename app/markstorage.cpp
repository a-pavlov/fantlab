#include "markstorage.h"
#include <QMap>
#include <QFile>
#include <QTextStream>

MarkStorage::MarkStorage() {

}

int MarkStorage::addUser(int user) {
    Q_ASSERT(user != 0);

    int userPos = -1;
    QMap<int, int>::const_iterator itr = userIndexes.find(user);
    if (itr == userIndexes.end()) {
        userPos = userIndexes.size();
        userIndexes.insert(user, userIndexes.size());
        markMatrix.append(QList<int>());
        for(int i = 0; i < workIndexes.size(); i++) markMatrix.back().append(0);
    } else {
        userPos = itr.value();
    }

    Q_ASSERT(markMatrix.size() == userIndexes.size());
    return userPos;
}

void MarkStorage::addMark(int user, int work, int mark) {
    Q_ASSERT(mark != 0);
    int userPos = addUser(user);
    Q_ASSERT(userPos != -1);
    Q_ASSERT(markMatrix.size() == userIndexes.size());

    int index = workIndexes.value(work, -1);

    if (index == -1) {
        // index not found - generate new
        index = workIndexes.size();
        workIndexes.insert(work, index);  // create index for book and store it in dictionary
        for(int i = 0; i < markMatrix.size(); ++i) {
            markMatrix[i].append(0);
        }
    }

    Q_ASSERT(index != -1);
    Q_ASSERT(index < markMatrix.at(userPos).size());
    Q_ASSERT(workIndexes.size() == markMatrix.at(0).size());
    Q_ASSERT(workIndexes.size() == markMatrix.at(userPos).size());
    markMatrix[userPos].replace(index, mark);
}

bool MarkStorage::saveData(const QString& worksFilename, const QString& marksFilename) const {

    // write books in sorted order
    QFile worksFile(worksFilename);

    if (worksFile.open(QFile::WriteOnly | QFile::Text)) {
        QStringList worksList;
        worksList.reserve(workIndexes.size());
        for(int i = 0; i < workIndexes.size(); ++i) worksList << QString();
        for(QMap<int, int>::const_iterator itr = workIndexes.constBegin(); itr != workIndexes.constEnd(); ++itr) worksList[itr.value()] = QString::number(itr.value() + 1) + "," + QString::number(itr.key());
        QTextStream fs(&worksFile);
        foreach(const QString& s, worksList) fs << s << "\n";
        worksFile.close();
    } else return false;

    // write marks - in columns - users in lines - marks
    //    u1  u2  u3  u4
    // m1 *   *   *   *
    // m2 *   *   *   *

    QFile marksFile(marksFilename);

    if (marksFile.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream fs(&marksFile);
        for(int i = 0; i < workIndexes.size(); ++i) fs << getMarksByWorkIndex(i).join(",") << "\n";
        marksFile.close();
    } else return false;

    return true;
}
