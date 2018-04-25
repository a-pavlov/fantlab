#include <QDebug>
#include <QStandardPaths>
#include <QFileDialog>
#include <QFile>
#include <QSortFilterProxyModel>
#include <QNetworkAccessManager>
#include <QMessageBox>
#include <QMainWindow>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <myhtml/api.h>

#include "mainwindow.h"
#include "htmlparser.h"
#include "cothinkermodel.h"
#include "octave.h"
#include "user.h"
#include "status_bar.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), nam(new QNetworkAccessManager(this)) {
    setupUi(this);
    setWindowIcon(QIcon(":/icons/computer.ico"));
    co_thinkers = new CoThinkerModel(nam, this);
    ct_sort = new QSortFilterProxyModel(this);
    ct_sort->setSourceModel(co_thinkers);
    ct_sort->setSortRole(CoThinkerModel::SortRole);
    ct_sort->setDynamicSortFilter(true);
    ctTree->setModel(ct_sort);
    ctTree->setRootIsDecorated(false);
    ctTree->setSortingEnabled(true);

    connect(ctTree->header(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)),
            this, SLOT(ctSortChanged(int, Qt::SortOrder)));

    menu->addAction(actionOpen);
    menu->addSeparator();
    menu->addAction(actionRequest);
    menu->addAction(actionCancel);
    menu->addSeparator();
    menu->addAction(actionRecommend);

    actionOpen->setEnabled(true);
    actionRequest->setEnabled(false);
    actionCancel->setEnabled(false);

    octave = new Octave(this);
    connect(octave, SIGNAL(iteration(int,QString)), this, SLOT(onIteration(int,QString)));

    connect(actionOpen, SIGNAL(triggered(bool)), this, SLOT(on_openFile(bool)));    
    connect(co_thinkers, SIGNAL(dataRefreshed(int,int)), this, SLOT(onRefreshCompleted(int,int)));

    sb = new StatusBar(this, QMainWindow::statusBar());
    sb->setIteration(0);
    sb->setCost("N/A");
}

MainWindow::~MainWindow() {}

mystatus_t serialization_callback(const char* data, size_t len, void* ctx) {
    Q_ASSERT(ctx != NULL);
    ((HtmlParser*)ctx)->exec(data, len);
    return MyCORE_STATUS_OK;
}

void MainWindow::on_openFile(bool) {
}

void MainWindow::ctSortChanged(int logicalIndex, Qt::SortOrder order) {
    ct_sort->sort(logicalIndex, order);
}

void MainWindow::onRefreshCompleted(int total, int error) {
    actionOpen->setEnabled(true);
    actionRequest->setEnabled(true);
    actionCancel->setEnabled(false);
    QMessageBox::information(this, tr("Refresh completed"), tr("Total users %1 with errors %2").arg(total).arg(error));
}

void MainWindow::on_actionOpen_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this
                                                    , tr("Open html page")
                                                    , QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)
                                                    , tr("html files (*.html)"));
    QByteArray html;

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            html = file.readAll();
        }
    }

    if (!html.isEmpty()) {
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
    actionRequest->setEnabled(co_thinkers->rowCount() > 0);
    QMessageBox::information(this
                             , tr("Import completed")
                             , tr("%1 records have been imported").arg(hp.getResults().size()));
}

void MainWindow::on_actionRequest_triggered() {
    actionRequest->setEnabled(false);
    actionCancel->setEnabled(true);
    co_thinkers->start();
}

void MainWindow::on_actionCancel_triggered() {
    actionRequest->setEnabled(true);
    actionCancel->setEnabled(false);
    co_thinkers->stop();
}

void MainWindow::on_actionRecommend_triggered() {
    octave->startOctave();
}

void MainWindow::onIteration(int step, QString cost) {
    sb->setIteration(step);
    sb->setCost(cost);
}
