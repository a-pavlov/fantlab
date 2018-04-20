#include "markstorage.h"

MarkStorage::MarkStorage() {

}

void MarkStorage::addMark(int user, int work, int mark) {
    //public void addMark(final String userName, final String bookName, int mark) {
    Q_ASSERT(mark != 0);
    if (users.isEmpty() || (users.at(users.size() - 1) != user)) {
        // register new user
        users.append(user);
        // no good solution
        markMatrix.append(QList<int>());
        for(int i = 0; i < workIndexes.size(); i++) markMatrix.back().append(0);
    }

    int userPos = markMatrix.size() - 1;
    Q_ASSERT(markMatrix.size() == users.size());
    int index = workIndexes.value(work, -1);

    if (index == -1) {
        // index not found - generate new
        index = workIndexes.size();
        workIndexes.insert(work, index);  // create index for book and store it in dictionary
        for(int i = 0; i < markMatrix.size(); ++i) {
            markMatrix[i].append(0);
        }

        //foreach(QList<int>& userMarks, marks) {
        //    usersMarks.add(0);
        //}
        //assert !marks.get(userPos).stream().filter(x -> x.compareTo(0) > 0).collect(Collectors.toList()).isEmpty();
    }


    Q_ASSERT(index != -1);
    Q_ASSERT(index < markMatrix.at(userPos).size());
    Q_ASSERT(workIndexes.size() == markMatrix.at(0).size());
    Q_ASSERT(workIndexes.size() == markMatrix.at(userPos).size());
    markMatrix[userPos].replace(index, mark);
}
