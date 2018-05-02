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
    QCOMPARE(1, wi.genres.size());
    QCOMPARE(1, wi.genres.at(0));
}

void TestJson::testWorkJsonNoFanGenre() {
    QFile f(":/data/work_no_genre");
    QVERIFY(f.open(QIODevice::ReadOnly));
    QByteArray data = f.readAll();
    f.close();
    QJsonDocument doc = QJsonDocument::fromJson(data, nullptr);
    WorkInfo wi = Misc::getWorkInfo(doc);
    QCOMPARE(QString("title"), wi.title);
    QCOMPARE(QString("name"), wi.name);
    QCOMPARE(QString("description"), wi.description);
    QCOMPARE(0, wi.genres.size());
}
