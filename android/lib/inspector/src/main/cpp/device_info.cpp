#include "device_info.h"
#include "jni_util.h"

using namespace std;

DeviceInfo getDeviceInfo(JNIEnv *env, jclass clazz) {
    jclass build_class = env->FindClass("android/os/Build");

    jfieldID manufacturer_id = env->GetStaticFieldID(build_class, "MANUFACTURER", "Ljava/lang/String;");
    jfieldID model_id = env->GetStaticFieldID(build_class, "MODEL", "Ljava/lang/String;");
    jmethodID getPackageName_id = env->GetStaticMethodID(clazz, "getPackageName", "()Ljava/lang/String;");
    jmethodID getVersionName_id = env->GetStaticMethodID(clazz, "getVersionName", "()Ljava/lang/String;");

    auto *jmanufacturer = (jstring) env->GetStaticObjectField(build_class, manufacturer_id);
    auto *jmodel = (jstring) env->GetStaticObjectField(build_class, model_id);
    auto *jpackageName = (jstring) env->CallStaticObjectMethod(clazz, getPackageName_id);
    auto *jversionName = (jstring) env->CallStaticObjectMethod(clazz, getVersionName_id);

    string manufacturer = readString(env, jmanufacturer);
    string model = readString(env, jmodel);
    string name = manufacturer != "unknown" ? manufacturer + " " + model : model;

    string packageName = readString(env, jpackageName);
    string versionName = readString(env, jversionName);

    return {"android", name, packageName, versionName};
}
