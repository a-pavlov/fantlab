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
    const QList<bool> indexes;
    void deleteOctave();
public:
    OctaveDlg(QWidget *parent, const MarkStorage& s, const QList<bool> i);
};

#endif // OCTAVEDLG_H
