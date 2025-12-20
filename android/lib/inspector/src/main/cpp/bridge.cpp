#include <jni.h>
#include <vector>
#include "jni_util.h"
#include "Inspector.h"
#include "AndroidDatabaseProvider.h"
#include "device_info.h"

using namespace std;
using json = nlohmann::json;

static JavaVM *jvm;
static unique_ptr<Inspector> inspector;

jint JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    jvm = vm;

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL
Java_br_pakerwreah_inspector_Inspector_initialize(JNIEnv *env, jclass clazz, jint port) {
    clazz = (jclass) env->NewGlobalRef(clazz);
    inspector = make_unique<Inspector>(make_shared<AndroidDatabaseProvider>(jvm, clazz), getDeviceInfo(env, clazz));
    // Emulator: ./adb forward tcp:30000 tcp:30000
    inspector->bind(port);
}

extern "C" JNIEXPORT void JNICALL
Java_br_pakerwreah_inspector_Inspector_setCipherKeyJNI(JNIEnv *env, jclass, jstring database, jstring password, jint version) {
    inspector->setCipherKey(readString(env, database), readString(env, password), version);
}

extern "C"
JNIEXPORT void JNICALL
Java_br_pakerwreah_inspector_Inspector_addPluginJNI(JNIEnv *env, jclass, jstring key, jstring name, jobject _plugin) {
    jobject plugin = env->NewGlobalRef(_plugin);

    inspector->addPlugin(readString(env, key), readString(env, name), [plugin] {
        JNIEnv *env = attachThread(jvm);

        jclass clazz = env->GetObjectClass(plugin);
        jmethodID methodID = env->GetMethodID(clazz, "action", "()Ljava/lang/String;");
        string res = readString(env, (jstring) env->CallObjectMethod(plugin, methodID));

        detachThread(jvm);

        return res;
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_br_pakerwreah_inspector_Inspector_addLivePluginJNI(JNIEnv *env, jclass, jstring key, jstring name, jobject _plugin) {
    jobject plugin = env->NewGlobalRef(_plugin);

    inspector->addLivePlugin(readString(env, key), readString(env, name), [plugin] {
        JNIEnv *env = attachThread(jvm);

        jclass clazz = env->GetObjectClass(plugin);
        jmethodID methodID = env->GetMethodID(clazz, "action", "()Ljava/lang/String;");
        string res = readString(env, (jstring) env->CallObjectMethod(plugin, methodID));

        detachThread(jvm);

        return res;
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_br_pakerwreah_inspector_Inspector_addPluginAPIJNI(JNIEnv *env, jclass, jstring method, jstring path, jobject _plugin) {
    auto *plugin = env->NewGlobalRef(_plugin);

    inspector->addPluginAPI(readString(env, method), readString(env, path), [plugin](const Params &params) {
        JNIEnv *env = attachThread(jvm);

        jclass clazz = env->GetObjectClass(plugin);
        jmethodID methodID = env->GetMethodID(clazz, "action", "(Ljava/lang/String;)[B");
        jstring jparams = env->NewStringUTF(json(params).dump().c_str());
        string res = readByteArray(env, (jbyteArray) env->CallObjectMethod(plugin, methodID, jparams));
        env->DeleteLocalRef(jparams);

        detachThread(jvm);

        return res;
    });
}

extern "C" JNIEXPORT jboolean JNICALL
Java_br_pakerwreah_inspector_NetworkInterceptor_isConnected(JNIEnv *, jobject) {
    return static_cast<jboolean>(inspector->isConnected());
}

extern "C" JNIEXPORT void JNICALL
Java_br_pakerwreah_inspector_NetworkInterceptor_sendRequest(JNIEnv *env, jobject, jstring uid, jstring headers, jbyteArray data) {
    inspector->sendRequest(readString(env, uid), readString(env, headers), readByteArray(env, data));
}

extern "C" JNIEXPORT void JNICALL
Java_br_pakerwreah_inspector_NetworkInterceptor_sendResponse(JNIEnv *env, jobject, jstring uid, jstring headers, jbyteArray data, jboolean compressed) {
    inspector->sendResponse(readString(env, uid), readString(env, headers), readByteArray(env, data), compressed);
}

extern "C"
JNIEXPORT void JNICALL
Java_br_pakerwreah_inspector_Inspector_sendMessageJNI(JNIEnv *env, jclass, jstring key, jstring message) {
    inspector->sendMessage(readString(env, key), readString(env, message));
}
