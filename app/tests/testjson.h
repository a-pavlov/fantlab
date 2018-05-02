#ifndef TESTJSON_H
#define TESTJSON_H

#include <QObject>

class TestJson : public QObject {
    Q_OBJECT
private slots:
    void testUtils();
    void testWorkJson();
    void testWorkJsonNoFanGenre();
};

#endif // TESTJSON_H
