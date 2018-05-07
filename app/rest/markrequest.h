#ifndef MARKREQUEST_H
#define MARKREQUEST_H

#include "request.h"

class MarkRequest : public Request {
private:
    int userId;
public:
    /**
     * @brief MarkRequest
     * @param user id of user for construct request
     * @param pr page parameter, will be emitted
     * @param parent
     */
    MarkRequest(int user, int pr, QObject* parent = 0);
    virtual QNetworkRequest getRequest() const;
};

#endif // MARKREQUEST_H
