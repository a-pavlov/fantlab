#ifndef TESTMARKSTORAGE_H
#define TESTMARKSTORAGE_H

#include <QObject>

class TestMarkStorage : public QObject
{
    Q_OBJECT
public:
    explicit TestMarkStorage(QObject *parent = nullptr);

signals:

private slots:
    void testStorageInit();
    void testStorageGetters();
    void testStorageArbitraryInput();
    void testAddUser();
    void testSetWorkMultipleTimes();
};

#endif // TESTMARKSTORAGE_H
