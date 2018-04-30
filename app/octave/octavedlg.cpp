#include <QDebug>

#include "octavedlg.h"
#include "octave.h"
#include "fs.h"


OctaveDlg::OctaveDlg(QWidget *parent): QDialog(parent)
    , octave(NULL)
    , totalLambdas(0)
    , finishedLambdas(0) {
    setupUi(this);
    connect(btnCancel, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(btnStart, SIGNAL(clicked(bool)), this, SLOT(onStart()));
}

void OctaveDlg::close() {
    if (octave != NULL) {
        octave->kill();
        qDebug() << "process killed";
    }
    accept();
}

void OctaveDlg::onStart() {
    totalLambdas = editLambdas->text().split(" ", QString::SkipEmptyParts).size();
    finishedLambdas = 0;
    progressBar->setRange(0, totalLambdas);
    progressBar->setValue(0);
    Utils::Fs::cleanDirectory(Utils::Fs::tempPath());
    Utils::Fs::copyDirectory(Utils::Fs::getOctaveScriptsPath(), Utils::Fs::tempPath());
    Octave* octave = new Octave(this);
    connect(octave, SIGNAL(lambda(QString)), this, SLOT(onLambda(QString)));
    connect(octave, SIGNAL(lambdaFinished()), this, SLOT(onLambdaFinished()));
    connect(octave, SIGNAL(iteration(int,QString)), this, SLOT(onCost(int,QString)));
    connect(octave, SIGNAL(minimalCost(QString)), this, SLOT(onMinCost(QString)));
    octave->startOctave();
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
