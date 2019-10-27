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

const char *InspectorID = "br/newm/inspector/Inspector";

class AndroidInpector : public Inspector {
protected:
    vector<string> databaseList() override {
        auto env = attachThread();

        jmethodID methodID = env->GetStaticMethodID(driver, "databaseList",
                                                    "()[Ljava/lang/String;");
        auto res = (jobjectArray) env->CallStaticObjectMethod(driver, methodID);

        int count = env->GetArrayLength(res);

        auto list = vector<string>();

        for (int i = 0; i < count; i++) {
            auto _name = (jstring) env->GetObjectArrayElement(res, i);
            const char *name = env->GetStringUTFChars(_name, nullptr);
            list.push_back(name);
            env->ReleaseStringUTFChars(_name, name);
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

    // No emulador precisa executar: ./adb forward tcp:30000 tcp:30000
    inspector->bind(30000);

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL
Java_br_newm_inspector_Inspector_initialize(JNIEnv *env, jobject /* this */) {
    inspector->preselectDB();
}