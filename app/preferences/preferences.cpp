#include "preferences.h"

const QString keyName   = "Common/Name";
const QString keyId     = "Common/Id";

void Preferences::setName(const QString& nm) {
    if (nm != value(keyName, "")) {
        setValue(keyName, nm);
        emit nameChanged(nm);
    }
}

QString Preferences::name() const {
    return value(keyName, "").toString();
}

int Preferences::getId() const {
    return value(keyId, 0).toInt();
}

void Preferences::setId(int id) {
    if (id != value(keyId, 0)) {
        setValue(keyId, id);
        emit idChanged(id);
    }
}
