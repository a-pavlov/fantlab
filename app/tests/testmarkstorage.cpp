#include "testmarkstorage.h"
#include "markstorage.h"

#include <QTest>

TestMarkStorage::TestMarkStorage(QObject *parent) : QObject(parent) {

}

void TestMarkStorage::testStorageInit() {
    MarkStorage accum;
    accum.addMark(1, 1, 10);
    accum.addMark(1, 2, 9);
    accum.addMark(1, 3, 4);
    QCOMPARE(accum.getWorkIndexes().size(), 3);
    QCOMPARE(accum.getUsers().size(), 1);
    QCOMPARE(accum.getMarkMatrix().size(), 1);
    QCOMPARE(accum.getMarkMatrix().at(0).size(), 3);

    accum.addMark(2, 1, 3);
    accum.addMark(2, 4, 10);
    QCOMPARE(accum.getWorkIndexes().size(), 4);
    QCOMPARE(accum.getUsers().size(), 2);
    QCOMPARE(accum.getMarkMatrix().size(), 2);
    QCOMPARE(accum.getMarkMatrix().at(0).size(), 4);
    QCOMPARE(accum.getMarkMatrix().at(1).size(), 4);

    accum.addMark(3, 5, 5);
    QCOMPARE(accum.getWorkIndexes().size(), 5);
    QCOMPARE(accum.getUsers().size(), 3);
    QCOMPARE(accum.getMarkMatrix().size(), 3);
    QCOMPARE(accum.getMarkMatrix().at(0).size(), 5);
    QCOMPARE(accum.getMarkMatrix().at(1).size(), 5);
    QCOMPARE(accum.getMarkMatrix().at(2).size(), 5);

    QCOMPARE(accum.getMarkMatrix().at(0).at(4), 0);
    QCOMPARE(accum.getMarkMatrix().at(1).at(4), 0);
    QCOMPARE(accum.getMarkMatrix().at(2).at(4), 5);

    for(int i = 1; i < 6; ++i)
        accum.addMark(4, i, i);

    QCOMPARE(accum.getWorkIndexes().size(), 5);
    QCOMPARE(accum.getUsers().size(), 4);
    QCOMPARE(accum.getMarkMatrix().size(), 4);

    for(int i = 0; i < 4; ++i)
        QCOMPARE(accum.getMarkMatrix().at(i).size(), 5);
}

void TestMarkStorage::testStorageGetters() {
    MarkStorage accum;
    accum.addMark(1, 1, 10);
    accum.addMark(1, 2, 9);
    accum.addMark(1, 3, 4);
    accum.addMark(2, 1, 3);
    accum.addMark(2, 4, 1);
    accum.addMark(3, 5, 8);
    QCOMPARE(accum.getTotalMarks(), 5);
    QList<bool> l1 = {true, true, true};
    QStringList book1 = accum.getMarksByWorkIndex(0, l1);
    QCOMPARE(book1.size(), 3);
    QCOMPARE(book1.at(0), QString("10"));
    QCOMPARE(book1.at(1), QString("3"));
    QCOMPARE(book1.at(2), QString("0"));
    QStringList book3 = accum.getMarksByWorkIndex(4, l1);

    QStringList book1_filter = accum.getMarksByWorkIndex(0, {true, true, false});
    QCOMPARE(2, book1_filter.size());
    QCOMPARE(book1_filter.at(0), QString("10"));
    QCOMPARE(book1_filter.at(1), QString("3"));

    QStringList book1_filter2 = accum.getMarksByWorkIndex(0, {false, true, false});
    QCOMPARE(1, book1_filter2.size());
    QCOMPARE(book1_filter2.at(0), QString("3"));

    QCOMPARE(0, accum.getMarksByWorkIndex(0, {false, false, false}).size());

    QCOMPARE(book3.size(), 3);
    QCOMPARE(book3.at(0), QString("0"));
    QCOMPARE(book3.at(1), QString("0"));
    QCOMPARE(book3.at(2), QString("8"));
}

void TestMarkStorage::testStorageArbitraryInput() {
    MarkStorage accum;
    accum.addMark(1, 1, 10);
    accum.addMark(2, 2, 9);
    accum.addMark(1, 2, 4);
    accum.addMark(2, 1, 9);
    QCOMPARE(accum.getWorkIndexes().size(), 2);
    QCOMPARE(accum.getUsers().size(), 2);
    QCOMPARE(accum.getMarkMatrix().size(), 2);
    accum.addMark(2, 3, 6);
    QCOMPARE(accum.getUsers().size(), 2);
    QCOMPARE(accum.getWorkIndexes().size(), 3);
    QCOMPARE(accum.getMarkMatrix().size(), 2);
    QCOMPARE(accum.getMarkMatrix().at(0).size(), 3);
    QCOMPARE(accum.getMarkMatrix().at(1).size(), 3);
}

void TestMarkStorage::testAddUser() {
    MarkStorage accum;
    QCOMPARE(accum.addUser(5), 0);
    QCOMPARE(accum.addUser(5), 0);
    QCOMPARE(accum.addUser(3), 1);
    QCOMPARE(accum.addUser(5), 0);
    QCOMPARE(accum.addUser(3), 1);
    QCOMPARE(accum.addUser(4), 2);
    accum.addMark(4, 2, 5);
    accum.addMark(4, 1, 6);
    accum.addMark(3, 2, 1);
    accum.addMark(5, 1, 10);
    accum.addMark(5, 2, 9);
    QCOMPARE(accum.getUsers().size(), 3);
    QCOMPARE(accum.getMarkMatrix().size(), 3);
    QCOMPARE(accum.getMarkMatrix().at(0).size(), 2);
    QCOMPARE(accum.getMarkMatrix().at(1).size(), 2);
    QCOMPARE(accum.getMarkMatrix().at(2).size(), 2);

    // validate user 5 first book is 2 second book is 1
    QCOMPARE(accum.getMarkMatrix().at(0).at(0), 9);
    QCOMPARE(accum.getMarkMatrix().at(0).at(1), 10);
}

void TestMarkStorage::testSetWorkMultipleTimes() {
    MarkStorage accum;
    QCOMPARE(accum.addUser(1), 0);
    QCOMPARE(accum.addUser(2), 1);
    accum.addMark(2, 1, 5);
    accum.addMark(2, 1, 6);
    accum.addMark(2, 1, 3);
    QCOMPARE(accum.getUsers().size(), 2);
    QCOMPARE(accum.getMarkMatrix().size(), 2);
    QCOMPARE(accum.getMarkMatrix().at(0).size(), 1);
    QCOMPARE(accum.getMarkMatrix().at(1).size(), 1);
    QCOMPARE(accum.getMarkMatrix().at(0).at(0), 0);
    QCOMPARE(accum.getMarkMatrix().at(1).at(0), 3);
    accum.addMark(1, 1, 10);
    accum.addMark(1, 1, 10);
    accum.addMark(1, 1, 9);
    QCOMPARE(accum.getMarkMatrix().at(0).size(), 1);
    QCOMPARE(accum.getMarkMatrix().at(1).size(), 1);
    QCOMPARE(accum.getMarkMatrix().at(0).at(0), 9);
    QCOMPARE(accum.getMarkMatrix().at(1).at(0), 3);
}
