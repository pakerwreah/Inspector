#ifndef INSPECTOR_JNIUTIL_H
#define INSPECTOR_JNIUTIL_H

#include <jni.h>
#include <string>

JNIEnv *attachThread(JavaVM *jvm);

void detachThread(JavaVM *jvm);

std::string readString(JNIEnv *env, jstring data);

std::string readByteArray(JNIEnv *env, jbyteArray data);

#endif //INSPECTOR_JNIUTIL_H