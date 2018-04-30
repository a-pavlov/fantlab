#include "testoctaveoutput.h"
#include "misc.h"
#include <QTest>

TestOctaveOutput::TestOctaveOutput(QObject *parent): QObject(parent) {

}

void TestOctaveOutput::testIterationsDetails() {
    QCOMPARE(Misc::octaveCost("   9 | Cost: 6.463457e+004 \r"), qMakePair(9, QString("6.463457e+004")));
    QCOMPARE(Misc::octaveCost("   10 | Cost: 6.432204e+004\r"), qMakePair(10, QString("6.432204e+004")));
    QCOMPARE(Misc::octaveCost("    1 | Cost: 1.531185e+005"), qMakePair(1, QString("1.531185e+005")));
    QCOMPARE(Misc::octaveCost("    1 | Cost: 1.531185   \r"), qMakePair(1, QString("1.531185")));
    QCOMPARE(Misc::octaveCost("    2234 | Cost: -1.531185e-005"), qMakePair(2234, QString("-1.531185e-005")));
    QCOMPARE(Misc::octaveCost("    61 | Cost: 1.531185"), qMakePair(61, QString("1.531185")));
}

void TestOctaveOutput::testIterationDetection() {
    QVERIFY(Misc::isIteration("Iteration"));
    QVERIFY(Misc::isIteration("Iteration  "));
    QVERIFY(Misc::isIteration("Iteration  \r"));
    QVERIFY(!Misc::isIteration("  Iteration"));
}

void TestOctaveOutput::testLambdaDetection() {
    QCOMPARE(Misc::octaveLambda("current_lambda: 100"), QString("100"));
    QCOMPARE(Misc::octaveLambda("current_lambda: 0.1"), QString("0.1"));
    QCOMPARE(Misc::octaveLambda("current_lambda:   0.0005"), QString("0.0005"));
    QCOMPARE(Misc::octaveLambda("current_lambda: 12.444789"), QString("12.444789"));
    QCOMPARE(Misc::octaveLambda("current_lambda: -1.07"), QString("-1.07"));
    QCOMPARE(Misc::octaveLambda("current_lambdas: 100"), QString());
    QCOMPARE(Misc::octaveLambda("current_lambda  100"), QString());
}
