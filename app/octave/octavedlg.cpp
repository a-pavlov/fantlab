#include "octavedlg.h"

OctaveDlg::OctaveDlg(QWidget *parent): QDialog(parent) {
    setupUi(this);
    connect(btnCancel, SIGNAL(clicked(bool)), this, SLOT(close()));
}

void OctaveDlg::close() {
    accept();
}
