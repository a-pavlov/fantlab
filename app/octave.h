#ifndef OCTAVE_H
#define OCTAVE_H

#include <QObject>
#include <QProcess>

class Octave : public QProcess {
    Q_OBJECT
public:
    Octave(QObject* parent = 0);
    void startOctave();
private slots:
    void octaveReadyReadStandardError();
    void octaveReadyReadStandardOutput();
signals:
    void iteration(int, QString);
};

#endif // OCTAVE_H
