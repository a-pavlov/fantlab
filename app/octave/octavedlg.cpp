#include <QDebug>

#include "octavedlg.h"
#include "octave.h"
#include "fs.h"
#include "cothinkermodel.h"


OctaveDlg::OctaveDlg(QWidget *parent, const CoThinkerModel& m): QDialog(parent)
    , totalLambdas(0)
    , finishedLambdas(0)
    , octave(NULL)
    , model(m) {
    setupUi(this);
    connect(btnCancel, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(btnStart, SIGNAL(clicked(bool)), this, SLOT(onStart()));
    connect(btnStop, SIGNAL(clicked(bool)), this, SLOT(onStop()));
    qDebug() << " users " << model.getMarkStorage().getMarkMatrix().size()
             << " marks " << model.getMarkStorage().getMarkMatrix().at(0).size();
}

void OctaveDlg::close() {
    accept();
}

void OctaveDlg::onStart() {
    totalLambdas = editLambdas->text().split(" ", QString::SkipEmptyParts).size();
    finishedLambdas = 0;
    progressBar->setRange(0, totalLambdas);
    progressBar->setValue(0);
    Utils::Fs::cleanDirectory(Utils::Fs::tempPath());
    Utils::Fs::copyDirectory(Utils::Fs::getOctaveScriptsPath(), Utils::Fs::tempPath());
    bool dataSaved = model.getMarkStorage().saveData(Utils::Fs::tempPath() + "works.csv", Utils::Fs::tempPath() + "marks.csv");
    if (dataSaved) {
        octave = new Octave(this);
        connect(octave, SIGNAL(lambda(QString)), this, SLOT(onLambda(QString)));
        connect(octave, SIGNAL(lambdaFinished()), this, SLOT(onLambdaFinished()));
        connect(octave, SIGNAL(iteration(int,QString)), this, SLOT(onCost(int,QString)));
        connect(octave, SIGNAL(minimalCost(QString)), this, SLOT(onMinCost(QString)));
        connect(octave, SIGNAL(started()), this, SLOT(onProcStarted()));
        connect(octave, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onProcFinished(int,QProcess::ExitStatus)));
        octave->startOctave();
    } else {
        lbProcStatus->setText(tr("Unable to save data files"));
    }
}

void OctaveDlg::onStop() {
    if (octave != NULL) {
        octave->kill();
    }
}

void OctaveDlg::onCost(int iteration, QString cost) {
    lbIter->setText(QString::number(iteration));
    lbCost->setText(cost);
}

void OctaveDlg::onMinCost(QString cost) {
    lbMinCost->setText(cost);
}

void OctaveDlg::onLambda(QString lambda) {
    lbLambda->setText(lambda);
}

void OctaveDlg::onLambdaFinished() {
    progressBar->setValue(++finishedLambdas);
}

void OctaveDlg::onProcStarted() {
    lbProcStatus->setText(tr("Running"));
}

void OctaveDlg::onProcFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode);
    lbProcStatus->setText(exitStatus==QProcess::NormalExit?tr("Done"):tr("Failed"));
}
