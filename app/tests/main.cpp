#include <QCoreApplication>
#include <QTest>

#include "testjson.h"
#include "testmarkstorage.h"
#include "testoctaveoutput.h"
#include "testcothinkermodel.h"

int main(int argc, char *argv[]) {
    freopen("testing.log", "w", stdout);
    QCoreApplication a(argc, argv);
    QTest::qExec(new TestJson, argc, argv);
    QTest::qExec(new TestMarkStorage, argc, argv);
    QTest::qExec(new TestOctaveOutput, argc, argv);
    QTest::qExec(new TestCoThinkerModel, argc, argv);
    return 0;
}
