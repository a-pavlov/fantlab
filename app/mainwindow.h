#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>

#include "ui_mainwindow.h"
#include "htmlparser.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT
private:
    HtmlParser hp;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_openFile(bool);
};

#endif // MAINWINDOW_H
