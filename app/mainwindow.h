#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>

#include "ui_mainwindow.h"
#include "htmlparser.h"

class CoThinkerModel;
class QSortFilterProxyModel;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT
private:
    HtmlParser hp;
    CoThinkerModel* co_thinkers;
    QSortFilterProxyModel* ct_sort;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_openFile(bool);
    void ctSortChanged(int logicalIndex, Qt::SortOrder order);
};

#endif // MAINWINDOW_H
