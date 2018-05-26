#include <QDebug>
#include <QStandardPaths>
#include <QFileDialog>
#include <QFile>
#include <QSortFilterProxyModel>
#include <QNetworkAccessManager>
#include <QMessageBox>
#include <QMainWindow>
#include <QInputDialog>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <myhtml/api.h>

#include "mainwindow.h"
#include "htmlparser.h"
#include "cothinkermodel.h"
#include "user.h"
#include "status_bar.h"
#include "fs.h"
#include "octavedlg.h"
#include "recommendmodel.h"
#include "preferences.h"

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
    ctTree->header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ctTree->header(), &QHeaderView::customContextMenuRequested, [=](const QPoint&) {
        QMenu hideshowColumn(this);
        hideshowColumn.setTitle(tr("Column visibility"));
        QList<QAction*> actions;
        for (int i=0; i < co_thinkers->columnCount(); ++i) {
            QAction *myAct = hideshowColumn.addAction(
                co_thinkers->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString());
            myAct->setCheckable(true);
            myAct->setChecked(!ctTree->isColumnHidden(i));
            actions.append(myAct);
        }
        // Call menu
        QAction *act = hideshowColumn.exec(QCursor::pos());
        if (act) {
            int col = actions.indexOf(act);
            Q_ASSERT(col >= 0);
            qDebug("Toggling column %d visibility", col);
            ctTree->setColumnHidden(col, !ctTree->isColumnHidden(col));
            if (!ctTree->isColumnHidden(col) && ctTree->columnWidth(col) <= 5)
                ctTree->setColumnWidth(col, 100);
        }

    });

    recommendations = new RecommendModel(this);
    recommendations_sort = new QSortFilterProxyModel(this);
    recommendations_sort->setSourceModel(recommendations);
    recommendations_sort->setSortRole(RecommendModel::SortRole);
    rTree->setModel(recommendations_sort);
    rTree->setRootIsDecorated(false);
    rTree->setSortingEnabled(true);
    recommendations->populate(Utils::Fs::tempPath() + "/recommendations.csv");

    connect(ctTree->header(), &QHeaderView::sortIndicatorChanged, [=](int logicalIndex, Qt::SortOrder order) {
        ct_sort->sort(logicalIndex, order);
    });

    menu->addAction(actionMyId);
    menu->addAction(actionOpen);
    menu->addSeparator();
    menu->addAction(actionRequest);
    menu->addAction(actionCancel);
    menu->addSeparator();
    menu->addAction(actionRecommend);

    Preferences pref;
    actionOpen->setEnabled(pref.hasId());
    actionRequest->setEnabled(false);
    actionCancel->setEnabled(false);

    connect(co_thinkers, &CoThinkerModel::dataRefreshed, [=](int total, int error) {
        actionOpen->setEnabled(true);
        actionRequest->setEnabled(true);
        actionCancel->setEnabled(false);
        QMessageBox::information(this, tr("Refresh completed"), tr("Total users %1 with errors %2").arg(total).arg(error));
    });

    sb = new StatusBar(this, QMainWindow::statusBar());
    sb->setId(pref.getId());
    sb->setCothinkersCount(0);
}

MainWindow::~MainWindow() {}

mystatus_t serialization_callback(const char* data, size_t len, void* ctx) {
    Q_ASSERT(ctx != NULL);
    ((HtmlParser*)ctx)->exec(data, len);
    return MyCORE_STATUS_OK;
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

    Preferences pref;

    QStringList self;
    self << ("http://fantlab.ru/user" + QString::number(pref.getId()))
         << "my_login_here"
         << "100"
         << "1.0";

    sb->setCothinkersCount(hp.getResults().count());
    hp.getResults().push_front(self);

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
    OctaveDlg dialog(this);
    dialog.exec();
}

void MainWindow::on_actionMyId_triggered() {
    Preferences pref;
    bool ok;
    int id = QInputDialog::getInt(this
                                  , tr("Self id")
                                  , tr("Set your id in fantlab site here")
                                  , 4500
                                  , 1
                                  , 999999
                                  , 1
                                  , &ok);
    if (ok) {
        pref.setId(id);
    } else if (!pref.hasId()) {
        QMessageBox::warning(this, tr("Recommendations problem"), tr("Unable to create recommendations without your id"));
    }

    actionOpen->setEnabled(pref.hasId());
    sb->setId(pref.getId());
}

void MainWindow::onIteration(int step, QString cost) {
    Q_UNUSED(step);
    Q_UNUSED(cost);
}
