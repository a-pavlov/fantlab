#ifndef WORKREQUEST_H
#define WORKREQUEST_H

#include "request.h"

class WorkRequest : public Request {
public:
    WorkRequest(int param, QObject* parent = 0);
    virtual QNetworkRequest getRequest() const;
};

#endif // WORKREQUEST_H
