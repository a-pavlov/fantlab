#include "status_bar.h"

#include <QStatusBar>
#include <QIcon>

StatusBar::StatusBar(QWidget *parent, QStatusBar *bar)
    : QWidget(parent) , m_bar(bar)
{
    setupUi(this);

    m_bar->addPermanentWidget(this, 1);
    m_bar->setContentsMargins(-1, -1, -1, -1);
    m_bar->setFixedHeight(21);


    /*imgConnected.addFile(res::statusConnected(), QSize(), QIcon::Normal, QIcon::Off);
    imgDisconnected.addFile(res::statusDisconnected(), QSize(), QIcon::Normal, QIcon::Off);

    Up0Down0.addFile(res::statusUD00(), QSize(), QIcon::Normal, QIcon::Off);
    Up0Down1.addFile(res::statusUD01(), QSize(), QIcon::Normal, QIcon::Off);
    Up1Down0.addFile(res::statusUD10(), QSize(), QIcon::Normal, QIcon::Off);
    Up1Down1.addFile(res::statusUD11(), QSize(), QIcon::Normal, QIcon::Off);

    labelInfoImg->setPixmap(QIcon(":/emule/common/User.ico").pixmap(16,16));

    reset();
    */
}

StatusBar::~StatusBar()
{
}

void StatusBar::setId(int id) {
    lbId->setText(tr("My ID: %1").arg(id));
}

void StatusBar::setCothinkersCount(int c) {
    lbCoThinkersCount->setText(tr("My co-thinkers count: %1").arg(c));
}

void StatusBar::setRequests(int total, int error) {
    Q_ASSERT(total >= 0);
    Q_ASSERT(error >= 0);
    labelReq->setText(tr("Requests: %1").arg(total));
    labelReqErr->setText(tr("Errors: %1").arg(error));
}

/*
void status_bar::setConnected(bool conn)
{
    if (conn)
    {
        labelNetImg->setPixmap(imgConnected.pixmap(16, 16));
        labelNet->setText(tr("eD2K:Connected"));
        labelNet->setToolTip(tr("eD2K:Connected"));
    }
    else
    {
        labelNetImg->setPixmap(imgDisconnected.pixmap(16, 16));
        labelNet->setText(tr("eD2K:Disconnected"));
        labelNet->setToolTip(tr("eD2K:Disconnected"));
        labelInfo->clear();
        labelServer->clear();
    }
}

void status_bar::setUpDown(unsigned long nUp, unsigned long nDown)
{
    QIcon icon = nUp > 0 ? (nDown > 0 ? Up1Down1 : Up1Down0) : (nDown > 0 ? Up0Down1 : Up0Down0);
    labelSpeedImg->setPixmap(icon.pixmap(16, 16));
    QString text = tr("Upload:") + QString(" %1/%2, ").arg(misc::friendlyUnit(nUp)).arg(tr("s")) +
                   tr("Download:") + QString(" %1/%2").arg(misc::friendlyUnit(nDown)).arg(tr("s"));
    labelSpeed->setText(text);
    labelSpeed->setToolTip(text);
}

void status_bar::setServerInfo(unsigned long nFiles, unsigned long nClients)
{
    QString strClients;
    QString strFiles;

    strClients.setNum(nClients);
    strFiles.setNum(nFiles);

    QString text = tr("Clients: ") + strClients + tr("|Files: ") + strFiles;
    labelInfo->setText(text);
    labelInfo->setToolTip(text);
}

void status_bar::setStatusMsg(QString strMsg)
{
    labelServer->setText(strMsg);
    labelServer->setToolTip(strMsg);
}

void status_bar::reset()
{
    setConnected(false);
    setUpDown(0, 0);
    setServerInfo(0, 0);
}
*/

