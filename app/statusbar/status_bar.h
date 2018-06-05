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
public:
    StatusBar(QWidget *parent, QStatusBar *bar);
    ~StatusBar();
    void setId(int);
    void setCothinkersCount(int);
    void setRequests(int total, int errors);
};

#endif // STATUS_BAR_H
