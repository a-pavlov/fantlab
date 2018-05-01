#ifndef OCTAVEDLG_H
#define OCTAVEDLG_H

#include <QObject>
#include "ui_octavedlg.h"

QT_BEGIN_NAMESPACE
class Octave;
QT_END_NAMESPACE

class OctaveDlg : public QDialog, public Ui::OctaveDlg {
    Q_OBJECT
private:
    int totalLambdas;
    int finishedLambdas;
    Octave* octave;
public:
    OctaveDlg(QWidget *parent);
private slots:
    void close();
    void onStart();
    void onStop();
    void onCost(int iteration, QString cost);
    void onMinCost(QString cost);
    void onLambda(QString lambda);
    void onLambdaFinished();
};

#endif // OCTAVEDLG_H
