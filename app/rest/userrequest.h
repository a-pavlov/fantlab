#ifndef USERREQUEST_H
#define USERREQUEST_H

#include "request.h"

class UserRequest : public Request {
public:
    UserRequest(int param, QObject* parent = 0);
    virtual QNetworkRequest getRequest() const;
};

#endif // USERREQUEST_H
