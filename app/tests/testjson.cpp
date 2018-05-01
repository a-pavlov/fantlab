#include <QTest>
#include <QTextStream>
#include <QJsonDocument>

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

void TestJson::testWorkJson() {
    QFile f(":/data/work");
    QVERIFY(f.open(QIODevice::ReadOnly));
    QByteArray data = f.readAll();
    f.close();
    QJsonDocument doc = QJsonDocument::fromJson(data, nullptr);
    WorkInfo wi = Misc::getWorkInfo(doc);
    QCOMPARE(QString("Вячеслав Рыбаков, Игорь Алимов «Дело победившей обезьяны»"), wi.title);
    QCOMPARE(QString("Дело победившей обезьяны"), wi.name);
    QVERIFY(!wi.description.isNull() && !wi.description.isEmpty());
}
