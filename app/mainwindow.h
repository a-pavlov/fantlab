#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>

#include "ui_mainwindow.h"
#include "htmlparser.h"

QT_BEGIN_NAMESPACE
class CoThinkerModel;
class QSortFilterProxyModel;
class Request;
class QNetworkAccessManager;
class StatusBar;
class RecommendModel;
QT_END_NAMESPACE

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT
private:
    HtmlParser hp;
    CoThinkerModel* co_thinkers;
    RecommendModel* recommendations;
    QSortFilterProxyModel* ct_sort;
    QSortFilterProxyModel* recommendations_sort;
    Request* user;
    QNetworkAccessManager* nam;
    StatusBar* sb;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();
    void on_actionRequest_triggered();
    void on_actionCancel_triggered();
    void on_actionRecommend_triggered();
    void on_actionMyId_triggered();

    void onIteration(int, QString);
};

#endif // MAINWINDOW_H
