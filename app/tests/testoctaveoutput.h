#ifndef TESTOCTAVEOUTPUT_H
#define TESTOCTAVEOUTPUT_H

#include <QObject>

class TestOctaveOutput: public QObject {
    Q_OBJECT
public:
    explicit TestOctaveOutput(QObject *parent = nullptr);
private slots:
    void testIterationsDetails();
    void testIterationDetection();
};

#endif // TESTOCTAVEOUTPUT_H
