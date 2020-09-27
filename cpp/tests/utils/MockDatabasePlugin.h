#ifndef INSPECTOR_MOCKDATABASEPLUGIN_H
#define INSPECTOR_MOCKDATABASEPLUGIN_H

#include "DatabasePlugin.h"

struct MockDatabasePlugin: public DatabasePlugin {
    using DatabasePlugin::DatabasePlugin;

    void setDebounce(std::chrono::nanoseconds debounce);
};

#endif //INSPECTOR_MOCKDATABASEPLUGIN_H
