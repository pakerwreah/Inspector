#include <jni.h>
#include <android/log.h>
#include <vector>
#include "Inspector.h"

using namespace std;

constexpr const char *LOG_TAG = "JNILog";

struct {
    void d(const char *str) { __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s", str); }

    void e(const char *str) { __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s", str); }
} Log;

JavaVM *jvm;
JNIEnv *env;
jclass driver;

JNIEnv *attachThread() {
    JNIEnv *env = nullptr;
    jvm->AttachCurrentThread(&env, nullptr);
    return env;
}

void detachThread() {
    jvm->DetachCurrentThread();
};

string readString(JNIEnv *env, jstring data) {
    const char *chars = env->GetStringUTFChars(data, nullptr);
    string str = chars;
    env->ReleaseStringUTFChars(data, chars);
    return str;
}

string readByteArray(JNIEnv *env, jbyteArray data) {
    int size = env->GetArrayLength(data);
    auto bytes = env->GetByteArrayElements(data, nullptr);
    auto str = string((const char *) bytes, size);
    env->ReleaseByteArrayElements(data, bytes, 0);
    return str;
}

const char *InspectorID = "br/newm/inspector/Inspector";

class AndroidInpector : public Inspector {
protected:
    vector<string> databaseList() override {
        auto env = attachThread();

        jmethodID methodID = env->GetStaticMethodID(driver, "databaseList", "()[Ljava/lang/String;");
        auto res = (jobjectArray) env->CallStaticObjectMethod(driver, methodID);

        int count = env->GetArrayLength(res);

        auto list = vector<string>();

        for (int i = 0; i < count; i++) {
            auto name = (jstring) env->GetObjectArrayElement(res, i);
            list.push_back(readString(env, name));
        }

        detachThread();

        return list;
    }
};

AndroidInpector *inspector;

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    jvm = vm;

    driver = env->FindClass(InspectorID);
    if (driver == nullptr)
        return JNI_ERR;

    driver = (jclass) env->NewGlobalRef(driver);

    inspector = new AndroidInpector;

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL
Java_br_newm_inspector_Inspector_initialize(JNIEnv *env, jobject, jint port) {
    // No emulador precisa executar: ./adb forward tcp:30000 tcp:30000
    inspector->bind(port);
}

extern "C" JNIEXPORT void JNICALL
Java_br_newm_inspector_Inspector_setCipherKeyJNI(JNIEnv *env, jobject, jstring database, jstring password, jint version) {
    inspector->setCipherKey(readString(env, database), readString(env, password), version);
}

extern "C" JNIEXPORT bool JNICALL
Java_br_newm_inspector_NetworkInterceptor_isConnected(JNIEnv *env, jobject) {
    return inspector->isConnected();
}

extern "C" JNIEXPORT void JNICALL
Java_br_newm_inspector_NetworkInterceptor_sendRequest(JNIEnv *env, jobject, jstring uid, jstring headers, jbyteArray data) {
    inspector->sendRequest(readString(env, uid), readString(env, headers), readByteArray(env, data));
}

extern "C" JNIEXPORT void JNICALL
Java_br_newm_inspector_NetworkInterceptor_sendResponse(JNIEnv *env, jobject, jstring uid, jstring headers, jbyteArray data, jboolean compressed) {
    inspector->sendResponse(readString(env, uid), readString(env, headers), readByteArray(env, data), (bool) compressed);
}