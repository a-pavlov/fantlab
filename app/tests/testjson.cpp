#include <QTest>
#include "testjson.h"
#include "misc.h"

void TestJson::testUtils() {
    QCOMPARE(Misc::url2UserId("https://fantlab/user456"), 456);
    QCOMPARE(Misc::url2UserId("https://fantlab/user1"), 1);
    QCOMPARE(Misc::url2UserId("https://fantlab/user123456789"), 123456789);
    QCOMPARE(Misc::url2UserId("xxxx/user123"), 123);
    QVERIFY(Misc::url2UserId("https://fantlab/users123456789") != 123456789);
    QVERIFY(Misc::url2UserId("https://fantlab\\user123456789") != 123456789);
}
