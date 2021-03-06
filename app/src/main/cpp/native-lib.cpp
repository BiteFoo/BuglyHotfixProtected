#include <jni.h>
#include <string>
#include <android/log.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "./include/checksign.h"
#include "check-sign.cpp"

//------------------------------------------------------
static int native_sum(int a,int b)
{
    return a+b;
}



static jstring getString(JNIEnv *env, jobject obj){
    return (env) -> NewStringUTF( "Native String Return");
}

static void native_setContext(JNIEnv* env,jobject context)
{
    if(context !=NULL)
    {

        LOGD("--------------->> ");
//        jobject  sysContext = getSystemContextByActityThread(env);
//        if(context == sysContext)
//        {
//            LOGD("[+] equal ");
//        } else{
//            LOGE("[-]  not equal  ===>> ");
//        }
    }
}

// com.loopher.bugly
#define JNIREG_CLASS "com/loopher/bugly/MainActivity"

static JNINativeMethod gMethods[] = {
        {"stringFromJNI", "()Ljava/lang/String;", (void*)getString},
        {"sum","(II)I",(void*) native_sum},
        {"setContext","(Landroid/content/Context;)V",(void*)native_setContext}

};

static int registerNativeMethods(JNIEnv* env, const char* className,
                                 JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if ((env)->RegisterNatives( clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env, JNIREG_CLASS, gMethods,
                               sizeof(gMethods) / sizeof(gMethods[0])))
        return JNI_FALSE;

    return JNI_TRUE;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    //
    JNIEnv* env = NULL;

    if ((vm)->GetEnv( (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    LOGD("JNI_OnLoad ------------->");
    if(env == NULL)
        return -1;
//    getContext(env);
    getSystemContextByActityThread(env); //获取到系统的context
//    getGlobalContext(env);
//    obtainContextByPackName(env,"com.loopher.bugly");
    if (!registerNatives(env)) {
        return -1;
    }
    return JNI_VERSION_1_4;
}




//extern "C"
//JNIEXPORT jstring
//JNICALL
//Java_com_loopher_bugly_MainActivity_stringFromJNI(
//        JNIEnv *env,
//        jobject /* this */) {
//    //bugly hotfix from jni
//    std::string hello = "OJBK <<----->>"; //
//    return env->NewStringUTF(hello.c_str());
//}
//extern "C"
//JNIEXPORT jint JNICALL
//Java_com_loopher_bugly_MainActivity_sum(JNIEnv *env, jobject instance, jint a, jint b) {
//
//    // TODO
//
//    return a+b;
//}


