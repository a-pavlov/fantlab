#ifndef OCTAVEDLG_H
#define OCTAVEDLG_H

#include <QObject>
#include <QProcess>
#include "ui_octavedlg.h"

QT_BEGIN_NAMESPACE
class Octave;
class MarkStorage;
QT_END_NAMESPACE

class OctaveDlg : public QDialog, public Ui::OctaveDlg {
    Q_OBJECT
private:
    int totalLambdas;
    int finishedLambdas;
    Octave* octave;
    const MarkStorage& stg;
    const QList<int> indexes;
public:
    OctaveDlg(QWidget *parent, const MarkStorage& s, const QList<int> i);
private slots:
    void close();
    void onStart();
    void onStop();
    void onCost(int iteration, QString cost);
    void onMinCost(QString cost);
    void onLambda(QString lambda);
    void onLambdaFinished();

    void onProcStarted();
    void onProcFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // OCTAVEDLG_H
