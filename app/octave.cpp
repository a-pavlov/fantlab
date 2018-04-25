#include <QDebug>
#include <QStringList>
#include <QDir>

#include "octave.h"
#include "misc.h"
#include "fs.h"

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
        if (Misc::isIteration(line)) continue;
        QPair<int, QString> cost = Misc::octaveCost(line);
        if (cost.first != 0 && !cost.second.isNull()) {
            emit iteration(cost.first, cost.second);
        } else
        qDebug() << "line " << line;
    }
}

void Octave::startOctave() {
    connect(this,SIGNAL(readyReadStandardOutput()),
                this,SLOT(octaveReadyReadStandardOutput()));
    qDebug() << "start " << (Utils::Fs::getOctavePath() + QDir::separator() + "octave-cli-4.2.2.exe")
             << "work dir " << Utils::Fs::tempPath();
    setWorkingDirectory(Utils::Fs::tempPath());
    start(Utils::Fs::getOctavePath() + QDir::separator() + "octave-cli-4.2.2.exe", QStringList() << "flrec.m");
}
