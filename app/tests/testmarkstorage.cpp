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
    QList<int> book1 = accum.getMarksByIndex(0);
    QCOMPARE(book1.size(), 3);
    QCOMPARE(book1.at(0), 10);
    QCOMPARE(book1.at(1), 3);
    QCOMPARE(book1.at(2), 0);
    QList<int> book3 = accum.getMarksByIndex(4);
    QCOMPARE(book3.size(), 3);
    QCOMPARE(book3.at(0), 0);
    QCOMPARE(book3.at(1), 0);
    QCOMPARE(book3.at(2), 8);
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
