#include "workdetailsdialog.h"
#include <QDesktopServices>
#include <QUrl>

WorkDetailsDialog::WorkDetailsDialog(
                    const QString& type
                  , const QString& title
                  , const QString& descr
                  , qint32 workId
                  , QWidget* parent): QDialog(parent) {

    setupUi(this);
    Q_UNUSED(type);
    plainDescr->appendPlainText(descr);
    lbLink->setText(QString("<a href=\"https://fantlab.ru/work%1\">%2</a>").arg(workId).arg(title));
    lbLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    connect(lbLink, &QLabel::linkActivated, [=](const QString& link) {
        QDesktopServices::openUrl(QUrl(link));
    });
}
