#include "user.h"
#include "cothinkermodel.h"
#include "userrequest.h"
#include "markrequest.h"
#include "workrequest.h"

#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonValueRef>
#include <QDebug>
#include <QPair>

User::User(bool myRec
           , const QString& u
           , const QString& nm
           , int pr
           , double sim
           , int id
           , int pos
           , CoThinkerModel* mod
           , QObject *parent): QObject(parent)
    , myRecord(myRec)
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
    , processedMarks(0)
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
    responseCount = o["responsecount"].toString().trimmed().toInt();
    descriptioncount = o["descriptioncount"].toString().trimmed().toInt();
    classifcount = o["classifcount"].toString().trimmed().toInt();
    votecount = o["votecount"].toString().trimmed().toInt();
    topicCount = o["topiccount"].toString().trimmed().toInt();
    messageCount = o["messagecount"].toString().trimmed().toInt();
    bookcase_count = o["bookcase_count"].toString().trimmed().toInt();
    curator_autors = o["curator_autors"].toString().trimmed().toInt();

    ticketsCount = o["tickets_count"].toString().trimmed().toInt();    
    status = tr("Finished");
    finishRequst();
}

void User::prepareMarksRequests() {
    // prepare mark requests
    int pageLastIndex = Misc2::divCeil(markCount, 200);
    //qDebug() << "mark count " << markCount << " last page index " << pageLastIndex;

    // pages start from 1
    for(int i = 1; i <= pageLastIndex; ++i) {
        //qDebug() << "add marks request " << i;
        pendingOperations.append([this,i]() mutable {
            Request* request = new MarkRequest(userId, i);
            connect(request, SIGNAL(finished(int, QJsonDocument)), this, SLOT(processMarksResponse(int,QJsonDocument)));
            connect(request, SIGNAL(jsonError(int, int)), this, SLOT(jsonError(int,int)));
            connect(request, SIGNAL(networkError(int, int)), this, SLOT(networkError(int,int)));
            request->start(model->getNetworkManager());
        });
    }
}

void User::processMarksResponse(int page, const QJsonDocument & jd) {
    Q_UNUSED(page);
    QJsonObject o = jd.object();
    QJsonArray a = o["items"].toArray();
    qDebug() << "user " << userId << " page " << page << " marks count " << a.size();
    for(const QJsonValueRef& ref: a) {
        if (ref.isObject()) {
            QJsonObject item = ref.toObject();
            int workId = item["work_id"].toInt();
            int mark = item["mark"].toString().trimmed().toInt();
            if (mark <= 0) {
                qWarning() << "mark for work " << workId << " has incorrect value " << mark;
                continue;
            }

            // check we have information about work
            const WorkInfo& wi = model->getWork(workId);
            // no information - prepare request and set it to the head of requests
            if (wi.isNull()) {
                //qDebug() << "add request work " << workId << " with mark " << mark;
                pendingWorkMarks.insert(workId, mark);

                pendingOperations.push_front([this,workId]() mutable {
                    Request* request = new WorkRequest(workId);
                    connect(request, SIGNAL(finished(int, QJsonDocument)), this, SLOT(processWorkResponse(int,QJsonDocument)));
                    connect(request, SIGNAL(jsonError(int, int)), this, SLOT(jsonError(int,int)));
                    connect(request, &Request::networkError, [=](int p,int e) {
                        Q_UNUSED(p);
                        qDebug() << "network error occurred param " << " error " << e;
                    });
                    request->start(model->getNetworkManager());
                });
            } else {
                if (Misc::isSF(wi)) model->getMarkStorage().addMark(userId, workId, mark);
                ++processedMarks;
            }
        }
    }

    finishRequst();
}

void User::processWorkResponse(int workId, const QJsonDocument& jd) {
    WorkInfo wi = Misc::getWorkInfo(jd);
    // cache work in dictionary
    model->addWork(workId, wi);
    // for SF store mark
    if (Misc::isSF(wi)) {
        int mark = pendingWorkMarks.value(workId, -1);
        Q_ASSERT(mark != -1);
        model->getMarkStorage().addMark(userId, workId, mark);
    }

    ++processedMarks;

    finishRequst();
}

void User::finishRequst() {
    if (pendingOperations.isEmpty()) model->deactivateUser(this);
    model->releaseRequestSlot();
    model->updateData(position);
}
