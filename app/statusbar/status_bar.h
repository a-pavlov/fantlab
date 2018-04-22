#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <QWidget>
#include "ui_status_bar.h"

QT_BEGIN_NAMESPACE
class QStatusBar;
QT_END_NAMESPACE

class StatusBar : public QWidget, public Ui::status_bar
{
    Q_OBJECT
    QStatusBar* m_bar;
    /*QIcon imgConnected;
    QIcon imgDisconnected;
    QIcon Up0Down0;
    QIcon Up1Down0;
    QIcon Up0Down1;
    QIcon Up1Down1;
    */
public:
    StatusBar(QWidget *parent, QStatusBar *bar);
    ~StatusBar();
    void setIteration(int step);
    void setCost(const QString&);
};

#endif // STATUS_BAR_H
