#include "jni_util.h"

using namespace std;

JNIEnv *attachThread(JavaVM *jvm) {
    JNIEnv *env = nullptr;
    jvm->AttachCurrentThread(&env, nullptr);
    return env;
}

void detachThread(JavaVM *jvm) {
    jvm->DetachCurrentThread();
}

string readString(JNIEnv *env, jstring data) {
    const char *chars = env->GetStringUTFChars(data, nullptr);
    string str = chars;
    env->ReleaseStringUTFChars(data, chars);
    return str;
}

string readByteArray(JNIEnv *env, jbyteArray data) {
    jsize size = env->GetArrayLength(data);
    jbyte *bytes = env->GetByteArrayElements(data, nullptr);
    auto str = string(reinterpret_cast<const char *>(bytes), static_cast<size_t>(size));
    env->ReleaseByteArrayElements(data, bytes, 0);
    return str;
}
