#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>

#include "ui_mainwindow.h"
#include "htmlparser.h"

class CoThinkerModel;
class QSortFilterProxyModel;
class Octave;
class Request;
class QNetworkAccessManager;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT
private:
    HtmlParser hp;
    CoThinkerModel* co_thinkers;
    QSortFilterProxyModel* ct_sort;
    Octave* octave;
    Request* user;
    QNetworkAccessManager* nam;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_openFile(bool);
    void ctSortChanged(int logicalIndex, Qt::SortOrder order);
    void on_btnOctaveClicked(bool);
    void on_btnSendClicked(bool);
    void onRefreshCompleted(int, int);
};

#endif // MAINWINDOW_H
