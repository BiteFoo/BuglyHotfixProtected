#include <jni.h>
#include <string>

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_loopher_bugly_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    //bugly hotfix from jni
    std::string hello = "OJBK <<----->>"; //
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_loopher_bugly_MainActivity_sum(JNIEnv *env, jobject instance, jint a, jint b) {

    // TODO

    return a+b;


}