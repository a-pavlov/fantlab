#ifndef TESTCOTHINKERMODEL_H
#define TESTCOTHINKERMODEL_H

#include <QObject>

class TestCoThinkerModel : public QObject
{
    Q_OBJECT
public:
    explicit TestCoThinkerModel(QObject *parent = nullptr);

private slots:
    void testPopulationAndSimilarity();
};

#endif // TESTCOTHINKERMODEL_H
