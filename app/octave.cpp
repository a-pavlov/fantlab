#include <QDebug>
#include <QStringList>

#include "octave.h"
#include "utils.h"

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
    QString data(this->readAllStandardOutput());
    foreach(const QString& line, data.split("\n", QString::SkipEmptyParts)) {
        if (Utils::isIteration(line)) continue;
        QPair<int, QString> cost = Utils::octaveCost(line);
        if (cost.first != 0 && !cost.second.isNull()) {
            emit iteration(cost.first, cost.second);
        } else
        qDebug() << "line " << line;
    }
}

void Octave::startOctave() {
    connect(this,SIGNAL(readyReadStandardOutput()),
                this,SLOT(octaveReadyReadStandardOutput()));
    setWorkingDirectory("C:\\dev\\octave-4.2.2\\bin\\data");
    start("C:\\dev\\octave-4.2.2\\bin\\octave-cli-4.2.2.exe", QStringList() << "flrec.m");
}
