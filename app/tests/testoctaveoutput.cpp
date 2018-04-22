#include "testoctaveoutput.h"
#include "utils.h"
#include <QTest>

TestOctaveOutput::TestOctaveOutput(QObject *parent): QObject(parent) {

}

void TestOctaveOutput::testIterationsDetails() {
    QCOMPARE(Utils::octaveCost("   9 | Cost: 6.463457e+004 \r"), qMakePair(9, QString("6.463457e+004")));
    QCOMPARE(Utils::octaveCost("   10 | Cost: 6.432204e+004\r"), qMakePair(10, QString("6.432204e+004")));
    QCOMPARE(Utils::octaveCost("    1 | Cost: 1.531185e+005"), qMakePair(1, QString("1.531185e+005")));
    QCOMPARE(Utils::octaveCost("    1 | Cost: 1.531185   \r"), qMakePair(1, QString("1.531185")));
    QCOMPARE(Utils::octaveCost("    2234 | Cost: -1.531185e-005"), qMakePair(2234, QString("-1.531185e-005")));
    QCOMPARE(Utils::octaveCost("    61 | Cost: 1.531185"), qMakePair(61, QString("1.531185")));
}

void TestOctaveOutput::testIterationDetection() {
    QVERIFY(Utils::isIteration("Iteration"));
    QVERIFY(Utils::isIteration("Iteration  "));
    QVERIFY(Utils::isIteration("Iteration  \r"));
    QVERIFY(!Utils::isIteration("  Iteration"));
}
