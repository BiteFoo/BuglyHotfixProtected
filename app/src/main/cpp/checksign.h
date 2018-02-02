#ifndef CHECKSIGN_H
#define CHECKSIGN_H

// void checkSign(JNIEnv* env, jobject thizz) __attribute__((__annotate__(("fla"))));
void checkSign(JNIEnv* env, jobject thizz) ;

jstring getPackageName(JNIEnv *env, jobject thizz);

jstring getSignInfo(JNIEnv* env, jobject thizz, jstring packageName);

jstring parseSignature(JNIEnv* env, jbyteArray signByteArr);

jstring byteToHexString(JNIEnv* env, jbyteArray signByteArr);

jstring md5Digest(JNIEnv* env, jstring input);

jstring getHexString(JNIEnv* env, jbyteArray inputByteArr, jclass stringClazz);

void unzipFileByName(JNIEnv* env, jstring apkFilePath, const char* subFile, const char* dir);

#endif
