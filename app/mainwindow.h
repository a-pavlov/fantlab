#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_expEdit_textChanged(const QString &arg1);

    void on_btnSend_clicked();

    void on_piwikReplyFinished(QNetworkReply::NetworkError code, QString errorString);
    void on_piwikReplyError(QNetworkReply::NetworkError code);
    void on_requestStarted(const QUrl& url);

    void on_promoReceived(int);
    void on_promoRestored(int);
};

#endif // MAINWINDOW_H
