#include <jni.h>
#include <vector>
#include "Inspector.h"

using namespace std;
using json = nlohmann::json;

JavaVM *jvm;

JNIEnv *attachThread() {
    JNIEnv *env = nullptr;
    jvm->AttachCurrentThread(&env, nullptr);
    return env;
}

void detachThread() {
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

string getDeviceName(JNIEnv *env) {
    jclass build_class = env->FindClass("android/os/Build");

    jfieldID manufacturer_id = env->GetStaticFieldID(build_class, "MANUFACTURER", "Ljava/lang/String;");
    jfieldID model_id = env->GetStaticFieldID(build_class, "MODEL", "Ljava/lang/String;");

    auto jmanufacturer = (jstring) env->GetStaticObjectField(build_class, manufacturer_id);
    auto jmodel = (jstring) env->GetStaticObjectField(build_class, model_id);

    string manufacturer = readString(env, jmanufacturer);
    string model = readString(env, jmodel);
    if (manufacturer != "unknown") {
        return manufacturer + " " + model;
    } else {
        return model;
    }
}

class AndroidDatabaseProvider : public DatabaseProvider {
    jclass clazz;
public:
    explicit AndroidDatabaseProvider(jclass clazz): clazz(clazz) {}
protected:
    vector<string> databasePathList() override {
        auto env = attachThread();

        jmethodID methodID = env->GetStaticMethodID(clazz, "databasePathList", "()[Ljava/lang/String;");
        auto res = (jobjectArray) env->CallStaticObjectMethod(clazz, methodID);

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

Inspector *inspector;

jint JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    jvm = vm;

    jclass clazz = env->FindClass("br/newm/inspector/Inspector");

    if (clazz == nullptr)
        return JNI_ERR;

    clazz = (jclass) env->NewGlobalRef(clazz);

    inspector = new Inspector(new AndroidDatabaseProvider(clazz), {"android", getDeviceName(env)});

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL
Java_br_newm_inspector_Inspector_initialize(JNIEnv *, jclass, jint port) {
    // Emulator: ./adb forward tcp:30000 tcp:30000
    inspector->bind(port);
}

extern "C" JNIEXPORT void JNICALL
Java_br_newm_inspector_Inspector_setCipherKeyJNI(JNIEnv *env, jclass, jstring database, jstring password, jint version) {
    inspector->setCipherKey(readString(env, database), readString(env, password), version);
}

extern "C"
JNIEXPORT void JNICALL
Java_br_newm_inspector_Inspector_addPluginJNI(JNIEnv *env, jclass, jstring key, jstring name, jobject _plugin) {
    auto plugin = env->NewGlobalRef(_plugin);

    inspector->addPlugin(readString(env, key), readString(env, name), [plugin] {
        auto env = attachThread();

        auto clazz = env->GetObjectClass(plugin);
        jmethodID methodID = env->GetMethodID(clazz, "action", "()Ljava/lang/String;");
        auto res = readString(env, (jstring) env->CallObjectMethod(plugin, methodID));

        detachThread();

        return res;
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_br_newm_inspector_Inspector_addLivePluginJNI(JNIEnv *env, jclass, jstring key, jstring name, jobject _plugin) {
    auto plugin = env->NewGlobalRef(_plugin);

    inspector->addLivePlugin(readString(env, key), readString(env, name), [plugin] {
        auto env = attachThread();

        auto clazz = env->GetObjectClass(plugin);
        jmethodID methodID = env->GetMethodID(clazz, "action", "()Ljava/lang/String;");
        auto res = readString(env, (jstring) env->CallObjectMethod(plugin, methodID));

        detachThread();

        return res;
    });
}

extern "C"
JNIEXPORT void JNICALL
Java_br_newm_inspector_Inspector_addPluginAPIJNI(JNIEnv *env, jclass, jstring method, jstring path, jobject _plugin) {
    auto plugin = env->NewGlobalRef(_plugin);

    inspector->addPluginAPI(readString(env, method), readString(env, path), [plugin](const Params &params) {
        auto env = attachThread();

        auto clazz = env->GetObjectClass(plugin);
        jmethodID methodID = env->GetMethodID(clazz, "action", "(Ljava/lang/String;)[B");
        jstring jparams = env->NewStringUTF(json(params).dump().c_str());
        auto res = readByteArray(env, (jbyteArray) env->CallObjectMethod(plugin, methodID, jparams));
        env->DeleteLocalRef(jparams);

        detachThread();

        return res;
    });
}

extern "C" JNIEXPORT jboolean JNICALL
Java_br_newm_inspector_NetworkInterceptor_isConnected(JNIEnv *, jobject) {
    return static_cast<jboolean>(inspector->isConnected());
}

extern "C" JNIEXPORT void JNICALL
Java_br_newm_inspector_NetworkInterceptor_sendRequest(JNIEnv *env, jobject, jstring uid, jstring headers, jbyteArray data) {
    inspector->sendRequest(readString(env, uid), readString(env, headers), readByteArray(env, data));
}

extern "C" JNIEXPORT void JNICALL
Java_br_newm_inspector_NetworkInterceptor_sendResponse(JNIEnv *env, jobject, jstring uid, jstring headers, jbyteArray data, jboolean compressed) {
    inspector->sendResponse(readString(env, uid), readString(env, headers), readByteArray(env, data), compressed);
}

extern "C"
JNIEXPORT void JNICALL
Java_br_newm_inspector_Inspector_sendMessageJNI(JNIEnv *env, jclass, jstring key, jstring message) {
    inspector->sendMessage(readString(env, key), readString(env, message));
}
