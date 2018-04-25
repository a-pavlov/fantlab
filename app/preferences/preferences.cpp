#include "preferences.h"

const QString keyName = "Common/Name";

void Preferences::setName(const QString& nm) {
    if (nm != value(keyName, "")) {
        setValue(keyName, nm);
        emit nameChanged(nm);
    }
}

QString Preferences::name() const {
    return value(keyName, "").toString();
}
