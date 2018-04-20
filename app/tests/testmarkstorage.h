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
};

#endif // TESTMARKSTORAGE_H
