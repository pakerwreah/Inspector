#include "MockDatabasePlugin.h"

using namespace std;

void MockDatabasePlugin::setDebounce(chrono::nanoseconds debounce) {
    this->debounce = debounce;
}
