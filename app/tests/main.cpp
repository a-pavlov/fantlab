#include <QCoreApplication>
#include <QTest>

#include "testjson.h"

int main(int argc, char *argv[]) {
    freopen("testing.log", "w", stdout);
    QCoreApplication a(argc, argv);
    QTest::qExec(new TestJson, argc, argv);
    return 0;
}
