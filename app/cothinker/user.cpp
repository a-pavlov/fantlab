#include "user.h"
#include "cothinkermodel.h"
#include "userrequest.h"
#include "markrequest.h"

#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>

User::User(const QString& u
           , const QString& nm
           , int pr
           , double sim
           , int id
           , int pos
           , CoThinkerModel* mod
           , QObject *parent): QObject(parent)
    , url(u)
    , name(nm)
    , pairs(pr)
    , similarity(sim)
    , userId(id)
    , position(pos)
    , markCount(0)
    , messageCount(0)
    , responseCount(0)
    , ticketsCount(0)
    , topicCount(0)
    , failCount(0)
    , errorCode(0)
    , model(mod) {
    pendingOperations.append([this]() mutable {
        Request* request = new UserRequest(this->userId);
        connect(request, SIGNAL(finished(int, QJsonDocument)), this, SLOT(processDetailsResponse(int,QJsonDocument)));
        connect(request, SIGNAL(jsonError(int, int)), this, SLOT(jsonError(int,int)));
        connect(request, SIGNAL(networkError(int, int)), this, SLOT(networkError(int,int)));
        request->start(this->model->getNetworkManager());
    });
}

User::~User() {
}

void User::requestData() {
    while(!pendingOperations.isEmpty() && model->requestSlots() > 0) {
        bool slotRequested = model->takeRequestSlot();
        Q_ASSERT(slotRequested);
        std::function<void()> oper = pendingOperations.takeFirst();
        oper();
    }
}

void User::jsonError(int param, int ec) {
    Q_UNUSED(param);
    errorCode = ec;
    status = tr("Json error %1").arg(ec);
    finishRequst();
}

void User::networkError(int param, int ec) {
    Q_UNUSED(param);
    errorCode = ec;
    status = tr("Network error %1").arg(ec);
    finishRequst();
}

void User::processDetailsResponse(int param, const QJsonDocument& jd) {
    Q_UNUSED(param);
    QJsonObject o = jd.object();
    login = o["login"].toString();
    className = o["class_name"].toString();
    markCount = o["markcount"].toString().trimmed().toInt();
    messageCount = o["messagecount"].toString().trimmed().toInt();
    responseCount = o["responsecount"].toString().trimmed().toInt();
    ticketsCount = o["tickets_count"].toString().trimmed().toInt();
    topicCount = o["topiccount"].toString().trimmed().toInt();
    status = tr("Finished");
    finishRequst();
    // prepare mark requests
    int pages = Misc2::divCeil(markCount, 50);
    for(int i = 0; i < pages; ++i) {
        /*
        pendingOperations.append([this,i]() mutable {
            Request* request = new MarkRequest(userId, i);
            connect(request, SIGNAL(finished(int, QJsonDocument)), this, SLOT(processMarksResponse(int,QJsonDocument)));
            connect(request, SIGNAL(jsonError(int, int)), this, SLOT(jsonError(int,int)));
            connect(request, SIGNAL(networkError(int, int)), this, SLOT(networkError(int,int)));
            request->start(model->getNetworkManager());
        });
        */
    }
}

void User::processMarksResponse(int, const QJsonDocument &) {
    // add pending work requests here
}

void User::processWorkResponse(int, const QJsonDocument &) {
    // update work response here
}

void User::finishRequst() {
    if (pendingOperations.isEmpty()) model->deactivateUser(this);
    model->releaseRequestSlot();
    model->updateData(position);
}
