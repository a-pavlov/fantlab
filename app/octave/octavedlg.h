#ifndef OCTAVEDLG_H
#define OCTAVEDLG_H

#include <QObject>
#include "ui_octavedlg.h"

class OctaveDlg : public QDialog, public Ui::OctaveDlg {
    Q_OBJECT
public:
    OctaveDlg(QWidget *parent);
private slots:
    void close();
};

#endif // OCTAVEDLG_H
