#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);

    /*ptracker = new PiwikTracker(NULL, QUrl("http://www.emuletorrent.com"), 1);
    connect(ptracker, SIGNAL(requestStarted(QUrl)), this, SLOT(on_requestStarted(QUrl)));
    connect(ptracker, SIGNAL(piwikReplyError(QNetworkReply::NetworkError)), this, SLOT(on_piwikReplyError(QNetworkReply::NetworkError)));
    connect(ptracker, SIGNAL(piwikReplyFinished(QNetworkReply::NetworkError,QString)), this, SLOT(on_piwikReplyFinished(QNetworkReply::NetworkError,QString)));
    QString loc = QLocale::system().name();
    if (loc.size() > 2) loc = loc.mid(0, 2);


#ifdef Q_OS_LINUX
    promo = new Promotions("/home/a-pavlov/dev/promoted.xml", "en");
#else
    promo = new Promotions("c:\\dev\\promoted.xml", "");
#endif

    connect(promo, SIGNAL(receivedItems(int)), this, SLOT(on_promoReceived(int)));
    connect(promo, SIGNAL(restoredItems(int)), this, SLOT(on_promoRestored(int)));
    promo->requestPromotions();
    */
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_expEdit_textChanged(const QString &arg1)
{
    btnSend->setDisabled(arg1.isEmpty());
}

void MainWindow::on_btnSend_clicked()
{

}

void MainWindow::on_piwikReplyFinished(QNetworkReply::NetworkError code, QString errorString) {

}

void MainWindow::on_piwikReplyError(QNetworkReply::NetworkError code) {

}

void MainWindow::on_requestStarted(const QUrl& url) {

}

void MainWindow::on_promoReceived(int c) {

}

void MainWindow::on_promoRestored(int c) {    
}
