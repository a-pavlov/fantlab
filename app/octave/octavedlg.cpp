#include <QDebug>

#include "octavedlg.h"
#include "octave.h"
#include "fs.h"
#include "markstorage.h"

void OctaveDlg::deleteOctave() {
    if (octave != nullptr) {
        octave->kill();
        octave->deleteLater();
    }

    octave = nullptr;
}

OctaveDlg::OctaveDlg(QWidget *parent, const MarkStorage& s, const QList<bool> i): QDialog(parent)
    , totalLambdas(0)
    , finishedLambdas(0)
    , octave(NULL)
    , stg(s)
    , indexes(i){
    setupUi(this);
    setWindowTitle(tr("Users %1").arg(indexes.size()));

    connect(this, &OctaveDlg::accepted, [=](){
        deleteOctave();
    });

    connect(this, &OctaveDlg::rejected, [=](){
        deleteOctave();
    });

    connect(btnOk, &QPushButton::clicked, [=](bool) {
        accept();
    });

    connect(btnCancel, &QPushButton::clicked, [=](bool) {
        deleteOctave();
        btnOk->setEnabled(true);
    });

    connect(btnStart, &QPushButton::clicked, [=](bool){
        totalLambdas = editLambdas->text().split(" ", QString::SkipEmptyParts).size();
        finishedLambdas = 0;
        progressBar->setRange(0, totalLambdas);
        progressBar->setValue(0);
        Utils::Fs::cleanDirectory(Utils::Fs::tempPath());
        Utils::Fs::copyDirectory(Utils::Fs::getOctaveScriptsPath(), Utils::Fs::tempPath());
        bool dataSaved = stg.saveData(Utils::Fs::tempPath() + "works.csv", Utils::Fs::tempPath() + "marks.csv", indexes);
        bool features = Utils::Fs::saveToFile(Utils::Fs::tempPath() + "num_features.txt", QString::number(spinFeatures->value()));
        bool iterations = Utils::Fs::saveToFile(Utils::Fs::tempPath() + "num_iterations.txt", QString::number(spinItr->value()));
        bool minPredicted = Utils::Fs::saveToFile(Utils::Fs::tempPath() + "min_predicted.txt", QString::number(spinMinMark->value()));
        bool lambdas = Utils::Fs::saveToFile(Utils::Fs::tempPath() + "lambdas.txt", editLambdas->text().replace("\\s+", "\n"));

        if (dataSaved && features && iterations && minPredicted && lambdas) {
            octave = new Octave(this);
            connect(octave, &Octave::lambda, [=](QString lambda) {
                lbLambda->setText(lambda);
            });

            connect(octave, &Octave::lambdaFinished, [=](){
                progressBar->setValue(++finishedLambdas);
            });

            connect(octave, &Octave::iteration, [=](int iteration, QString cost){
                lbIter->setText(QString::number(iteration));
                lbCost->setText(cost);
            });

            connect(octave, &Octave::minimalCost, [=](QString cost){
                lbMinCost->setText(cost);
            });

            connect(octave, &Octave::started, [=](){
                lbProcStatus->setText(tr("Running"));
            });

            connect(octave, static_cast<void (Octave::*)(int, QProcess::ExitStatus)>(&Octave::finished), [=](int exitCode, QProcess::ExitStatus exitStatus) {
                Q_UNUSED(exitCode);
                lbProcStatus->setText(exitStatus==QProcess::NormalExit?tr("Done"):tr("Failed"));
                btnOk->setEnabled(true);
                btnCancel->setEnabled(false);
            });

            octave->startOctave();
            btnCancel->setEnabled(true);
            btnOk->setEnabled(false);
        } else {
            lbProcStatus->setText(tr("Unable to save data files"));
        }
    });

    //connect(btnCancel, SIGNAL(clicked(bool)), this, SLOT(close()));
    //connect(btnStop, SIGNAL(clicked(bool)), this, SLOT(onStop()));
    qDebug() << " users " << stg.getMarkMatrix().size()
             << " marks " << stg.getMarkMatrix().at(0).size();
}
