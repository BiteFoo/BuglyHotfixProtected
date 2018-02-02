//
// Created by John.Lu on 2018/2/2.
//

#ifndef BUGLYHOTFIXPROTECTED_CHECKSIGN_H
#define BUGLYHOTFIXPROTECTED_CHECKSIGN_H
#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include <string>

#define LOG_TAG "Loopher"
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##args)

static jobject getGlobalContext(JNIEnv *env);
static void obtainContextByPackName(JNIEnv* env,const char* pkgName);
static jobject  getContext(JNIEnv* env);
static jobject getSystemContextByActityThread(JNIEnv* env);
static void getSign(JNIEnv *env,jobject context);

#endif //BUGLYHOTFIXPROTECTED_CHECKSIGN_H
