#include "markstorage.h"

MarkStorage::MarkStorage() {

}

void MarkStorage::addMark(int user, int work, int mark) {
    //public void addMark(final String userName, final String bookName, int mark) {
    Q_ASSERT(mark != 0);
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
