#include "testcothinkermodel.h"
#include "cothinkermodel.h"
#include <QTest>

TestCoThinkerModel::TestCoThinkerModel(QObject *parent) : QObject(parent) {}

void TestCoThinkerModel::testPopulationAndSimilarity() {
    CoThinkerModel model(nullptr);
    QList<QStringList> input;
    input.append(QStringList() << "x" << "y" << "20" << "0.67");
    input.append(QStringList() << "x" << "y" << "30" << "0.60");
    input.append(QStringList() << "x" << "y" << "10" << "0.33");
    input.append(QStringList() << "x" << "y" << "50" << "-0.02");
    model.populate(input);
    QCOMPARE(model.rowCount(), input.size());
    QCOMPARE(model.getMinSim(), 0.33);
    QList<User*> set1 = model.getSimilarUsers(0.33);
    QCOMPARE(set1.size(), 3);
    QList<User*> set2 = model.getSimilarUsers(0.59);
    QCOMPARE(set2.size(), 2);
}

