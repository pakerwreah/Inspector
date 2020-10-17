#ifndef INSPECTOR_DATABASEPROVIDER_H
#define INSPECTOR_DATABASEPROVIDER_H

#include "Inspector.h"
#include <jni.h>

class AndroidDatabaseProvider : public DatabaseProvider {
private:
    JavaVM *jvm;
    jclass clazz;

public:
    AndroidDatabaseProvider(JavaVM *jvm, jclass clazz);

protected:
    std::vector<std::string> databasePathList() const override;
};

#endif //INSPECTOR_DATABASEPROVIDER_H