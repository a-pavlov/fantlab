#include "mainwindow.h"
#include <QDebug>
#include <QStandardPaths>
#include <QFileDialog>
#include <QFile>
#include <QSortFilterProxyModel>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <myhtml/api.h>

#include "htmlparser.h"
#include "cothinkermodel.h"
#include "octave.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{    
    setupUi(this);
    co_thinkers = new CoThinkerModel(this);
    ct_sort = new QSortFilterProxyModel(this);
    ct_sort->setSourceModel(co_thinkers);
    ct_sort->setSortRole(CoThinkerModel::SortRole);
    ct_sort->setDynamicSortFilter(true);
    ctTree->setModel(ct_sort);
    ctTree->setRootIsDecorated(false);
    ctTree->setSortingEnabled(true);

    connect(ctTree->header(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)),
            this, SLOT(ctSortChanged(int, Qt::SortOrder)));

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

    octave = new Octave(this);

    connect(actionOpen, SIGNAL(triggered(bool)), this, SLOT(on_openFile(bool)));
    connect(btnOctave, SIGNAL(clicked(bool)), this, SLOT(on_btnOctaveClicked(bool)));
}

MainWindow::~MainWindow()
{
}

mystatus_t serialization_callback(const char* data, size_t len, void* ctx) {
    Q_ASSERT(ctx != NULL);
    ((HtmlParser*)ctx)->exec(data, len);
    return MyCORE_STATUS_OK;
}

void MainWindow::on_openFile(bool)
{    
    QString fileName = QFileDialog::getOpenFileName(this
                                                    , tr("Open html page")
                                                    , QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)
                                                    , tr("html files (*.html)"));
    qDebug() << "file name " << fileName;
    QByteArray html;

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            html = file.readAll();
        }
    }

    if (!html.isEmpty()){
        // basic init
        myhtml_t* myhtml = myhtml_create();
        myhtml_init(myhtml, MyHTML_OPTIONS_DEFAULT, 1, 0);

        // first tree init
        myhtml_tree_t* tree = myhtml_tree_create();
        myhtml_tree_init(tree, myhtml);

        myhtml_tree_parse_flags_set(tree, static_cast<myhtml_tree_parse_flags_t>(MyHTML_TREE_PARSE_FLAGS_WITHOUT_DOCTYPE_IN_TREE | MyHTML_TREE_PARSE_FLAGS_SKIP_WHITESPACE_TOKEN));
        myhtml_parse(tree, MyENCODING_UTF_8, html.constData(), html.size());
        myhtml_serialization_tree_callback(myhtml_tree_get_node_html(tree), serialization_callback, &hp);
        myhtml_tree_destroy(tree);
        myhtml_destroy(myhtml);
    }

    co_thinkers->populate(hp.getResults());
}

void MainWindow::ctSortChanged(int logicalIndex, Qt::SortOrder order) {
    ct_sort->sort(logicalIndex, order);
}

void MainWindow::on_btnOctaveClicked(bool)
{
    qDebug() << Q_FUNC_INFO;
    octave->startOctave();

}
