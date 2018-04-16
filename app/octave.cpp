#include "octave.h"
#include <QDebug>
#include <QStringList>

Octave::Octave(QObject* parent): QProcess(parent) {
    connect(this,SIGNAL(readyReadStandardError()),
                this,SLOT(octaveReadyReadStandardError()));
    connect(this,SIGNAL(readyReadStandardOutput()),
                this,SLOT(octaveReadyReadStandardOutput()));
}

void Octave::octaveReadyReadStandardError() {
    qDebug() << this->readAllStandardError();
}

void Octave::octaveReadyReadStandardOutput() {
    qDebug() << this->readAllStandardOutput();
}

void Octave::startOctave() {
    connect(this,SIGNAL(readyReadStandardOutput()),
                this,SLOT(octaveReadyReadStandardOutput()));
    setWorkingDirectory("C:\\dev\\octave-4.2.2\\bin\\data");
    start("C:\\dev\\octave-4.2.2\\bin\\octave-cli-4.2.2.exe", QStringList() << "flrec.m");
}
