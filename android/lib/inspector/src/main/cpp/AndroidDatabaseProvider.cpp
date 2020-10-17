#include "AndroidDatabaseProvider.h"
#include "jni_util.h"

using namespace std;

AndroidDatabaseProvider::AndroidDatabaseProvider(JavaVM *jvm, jclass clazz) : jvm(jvm), clazz(clazz) {}

vector <string> AndroidDatabaseProvider::databasePathList() const {
    JNIEnv *env = attachThread(jvm);

    jmethodID methodID = env->GetStaticMethodID(clazz, "databasePathList", "()[Ljava/lang/String;");
    auto *res = (jobjectArray) env->CallStaticObjectMethod(clazz, methodID);

    int count = env->GetArrayLength(res);

    vector<string> list;

    for (int i = 0; i < count; i++) {
        auto *name = (jstring) env->GetObjectArrayElement(res, i);
        list.push_back(readString(env, name));
    }

    detachThread(jvm);

    return list;
}
