#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QString>
#include <QDebug>
#include "qinisettings.h"
#include "misc.h"

class Preferences : public QIniSettings {
  Q_OBJECT
  Q_DISABLE_COPY(Preferences)

public:
  Preferences() : QIniSettings("fantlab",  "app"){}

public:
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    void setName(const QString&);
    QString name() const;
signals:
    void nameChanged(const QString& nm);
};

#endif // PREFERENCES_H
