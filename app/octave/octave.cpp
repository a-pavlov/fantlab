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
    QString input(this->readAllStandardOutput());

    // to avoid uncontrolled buffer's size increment just reset it if length too big
    if (lastLine.length() > 100) lastLine.clear();
    QString data = lastLine + input;
    lastLine.clear();

    foreach(const QString& line, data.split("\n", QString::SkipEmptyParts)) {
        if (Misc::isIteration(line)) {
            continue;
        }

        if (Misc::isLambdaFinished(line)) {
            emit lambdaFinished();
            continue;
        }

        QPair<int, QString> cost = Misc::octaveCost(line);
        if (cost.first != 0 && !cost.second.isNull()) {
            emit iteration(cost.first, cost.second);
            continue;
        }

        QString minCost = Misc::octaveMinCost(line);

        if (!minCost.isNull()) {
            emit minimalCost(minCost);
            continue;
        }

        QString ld = Misc::octaveLambda(line);

        if (!ld.isNull()) {
            emit lambda(ld);
            continue;
        }

        qDebug() << "line " << line;
        lastLine = line;
    }
}

void Octave::startOctave() {
    connect(this,SIGNAL(readyReadStandardOutput()),
                this,SLOT(octaveReadyReadStandardOutput()));
#ifdef Q_OS_LINUX
    qDebug() << "start " << (Utils::Fs::getOctavePath() + QDir::separator() + "octave")
             << "work dir " << Utils::Fs::tempPath();
    setWorkingDirectory(Utils::Fs::tempPath());
    start("octave", QStringList() << "flrec.m");
#else
    qDebug() << "start " << (Utils::Fs::getOctavePath() + QDir::separator() + "octave-cli-4.2.2.exe")
             << "work dir " << Utils::Fs::tempPath();
    setWorkingDirectory(Utils::Fs::tempPath());
    start(Utils::Fs::getOctavePath() + QDir::separator() + "bin\\octave-cli.exe", QStringList() << "flrec.m");
#endif
}
