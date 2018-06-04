#ifndef WORKDETAILSDIALOG_H
#define WORKDETAILSDIALOG_H

#include <QObject>
#include <QDialog>
#include "ui_workdetailsdialog.h"

class WorkDetailsDialog : public QDialog, public Ui::WorkDetailsDialog {
    Q_OBJECT
private:
public:
    WorkDetailsDialog(const QString& type
                      , const QString& title
                      , const QString& descr
                      , qint32 workId
                      , QWidget* parent);
};

#endif // WORKDETAILSDIALOG_H
