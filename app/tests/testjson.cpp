#include <QTest>
#include "testjson.h"
#include "utils.h"

void TestJson::testUtils() {
    QCOMPARE(Utils::url2UserId("https://fantlab/user456"), 456);
    QCOMPARE(Utils::url2UserId("https://fantlab/user1"), 1);
    QCOMPARE(Utils::url2UserId("https://fantlab/user123456789"), 123456789);
    QCOMPARE(Utils::url2UserId("xxxx/user123"), 123);
    QVERIFY(Utils::url2UserId("https://fantlab/users123456789") != 123456789);
    QVERIFY(Utils::url2UserId("https://fantlab\\user123456789") != 123456789);
}
