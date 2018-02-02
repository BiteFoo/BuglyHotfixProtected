#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include "checksign.h"
#include <string.h>
#include "md5.h"
#include "mlog.h"
#include "exception_helper.h"

char *MD5_file(const char *path, int md5_len);

/**
 * 检测AndroidManifest.xml的MD5值
 */
void checkAndroidManifestMd5(const char* manifestFile) {
	const char* configFileMd5 = "CONFIG_FILE_MD5";
	char* manifestFileMd5 =  MD5_file(manifestFile, 32);
	remove(manifestFile); //删除AndroidManifest.xml文件
	if (strcmp(configFileMd5, "CONFIG_FILE_MD5") != 0 && strcmp(configFileMd5, manifestFileMd5) != 0) {
		exit(0);
	}
}

/**
 * 获取包名
 */
jstring getPackageName(JNIEnv *env, jobject thizz){
	//获取包名
	jclass packageManagerClazz = env->FindClass(
				"android/content/pm/PackageManager");
	jclass applicationClazz = env->FindClass("android/app/Application");
	jmethodID getPackageNameMethodID = env->GetMethodID(
				applicationClazz, "getPackageName", "()Ljava/lang/String;");
	jstring package = (jstring) (env->CallObjectMethod(thizz,
				getPackageNameMethodID));
	releaseLocalObjRef(env, &packageManagerClazz);
	releaseLocalObjRef(env, &applicationClazz);
	return package;
}

/**
 * 获取签名文件签名
 */
void checkSign(JNIEnv* env, jobject thizz)  {
	const char* appSign = "APP_SIGN";
	jstring packageName = getPackageName(env, thizz);
	jstring signInfo = getSignInfo(env, thizz, packageName);
	const char* str = env->GetStringUTFChars(signInfo, JNI_FALSE);
	if (strcmp(appSign, "APP_SIGN") != 0 && strcmp(appSign, str) != 0) {
	    //LOGD("[x] call checkSign exit newSign =%s ,old =%s ",str,appSign);
		exit(0);
	}
	env->ReleaseStringUTFChars(signInfo, str);
	env->DeleteLocalRef(signInfo);
}

jstring getSignInfo(JNIEnv* env, jobject thizz, jstring packageName) {
	env->PushLocalFrame(128);
	jclass wrapperClazz = env->FindClass("android/content/ContextWrapper");
	jmethodID packageManagerMethodId = env->GetMethodID(wrapperClazz,
			"getPackageManager", "()Landroid/content/pm/PackageManager;");
	jobject packageManager = env->CallObjectMethod(thizz,
			packageManagerMethodId);

	jclass packageManagerClazz = env->FindClass("android/content/pm/PackageManager");
	jfieldID getSignaturesFieldId = env->GetStaticFieldID(packageManagerClazz, "GET_SIGNATURES", "I");
	jint getSignatures = env->GetStaticIntField(packageManagerClazz,
			getSignaturesFieldId);
	jmethodID packageInfoMethodId = env->GetMethodID(packageManagerClazz, "getPackageInfo",
			"(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
	jobject packageInfo = env->CallObjectMethod(packageManager,
			packageInfoMethodId, packageName, getSignatures);

	jclass packageInfoClazz = env->FindClass("android/content/pm/PackageInfo");
	jfieldID signaturesFieldId = env->GetFieldID(packageInfoClazz,
			"signatures", "[Landroid/content/pm/Signature;");
	jobject signatureArr = env->GetObjectField(packageInfo,
			signaturesFieldId);

	jobject signature = env->GetObjectArrayElement((jobjectArray)signatureArr, 0);
	jclass signatureClazz = env->FindClass("android/content/pm/Signature");
	jmethodID toByteArrayMethodId = env->GetMethodID(signatureClazz,
			"toByteArray", "()[B");
	jbyteArray signByteArr = (jbyteArray)env->CallObjectMethod(signature, toByteArrayMethodId);
	jobject parstSign = parseSignature(env, signByteArr);

	env->ExceptionClear();
	parstSign = env->PopLocalFrame(parstSign);
	return (jstring)parstSign;
}

jstring parseSignature(JNIEnv* env, jbyteArray signByteArr) {
	env->PushLocalFrame(128);
	jstring X_509 = env->NewStringUTF("X.509");
	jclass certFactoryClazz = env->FindClass("java/security/cert/CertificateFactory");
	jmethodID getInstanceMethodId = env->GetStaticMethodID(certFactoryClazz, "getInstance",
			"(Ljava/lang/String;)Ljava/security/cert/CertificateFactory;");
	jobject certFactory = env->CallStaticObjectMethod(certFactoryClazz,
			getInstanceMethodId, X_509);

	jclass byteArrInputClazz = env->FindClass("java/io/ByteArrayInputStream");
	jmethodID initArrInputMethodId = env->GetMethodID(byteArrInputClazz,
			"<init>", "([B)V");
	jobject byteArrInput = env->NewObject(byteArrInputClazz,
			initArrInputMethodId, signByteArr);

	jmethodID generateCerMethodId = env->GetMethodID(certFactoryClazz,
			"generateCertificate",
			"(Ljava/io/InputStream;)Ljava/security/cert/Certificate;");
	jobject X509Cert = env->CallObjectMethod(certFactory,
			generateCerMethodId, byteArrInput);

	jclass certClazz = env->FindClass("java/security/cert/Certificate");
	jmethodID getEncodeMethodId = env->GetMethodID(certClazz,
			"getEncoded", "()[B");
	jbyteArray encodeArr = (jbyteArray)env->CallObjectMethod(X509Cert,
			getEncodeMethodId);

	jstring publicKey = byteToHexString(env, encodeArr);

	jobject md5Str = md5Digest(env, publicKey);
	env->ExceptionClear();
	md5Str = env->PopLocalFrame(md5Str);
	return (jstring)md5Str;
}

jstring byteToHexString(JNIEnv* env, jbyteArray signByteArr) {
	env->PushLocalFrame(128);
	int i = 0;
	jint arrLen = env->GetArrayLength(signByteArr);
	char strArr[arrLen];
	jclass intClazz = env->FindClass("java/lang/Integer");
	jmethodID toHexStrMethodId = env->GetStaticMethodID(intClazz,
			"toHexString", "(I)Ljava/lang/String;");

	jclass sbClazz = env->FindClass("java/lang/StringBuffer");
	jmethodID initSbMethodId = env->GetMethodID(sbClazz, "<init>",
			"(I)V");
	jobject sb = env->NewObject(sbClazz, initSbMethodId, arrLen);

	jmethodID appendMethonId = env->GetMethodID(sbClazz, "append",
			"(Ljava/lang/String;)Ljava/lang/StringBuffer;");

	jstring zero = env->NewStringUTF("0");
	jbyte tmpArr[arrLen];
	jobject tmpStr;
	env->GetByteArrayRegion(signByteArr, 0, arrLen, tmpArr);
	for (i = 0; i < 50; i++) {
		tmpStr = env->CallStaticObjectMethod(intClazz, toHexStrMethodId,
				0xFF & (char) tmpArr[i]);
		jsize strLen = env->GetStringUTFLength((jstring)tmpStr);
		if (strLen < 2) {
			env->CallObjectMethod(sb, appendMethonId, zero);
		}
		env->CallObjectMethod(sb, appendMethonId, tmpStr);
		env->DeleteLocalRef(tmpStr);
	}

	jmethodID toStrMethodId = env->GetMethodID(sbClazz, "toString",
			"()Ljava/lang/String;");

	jobject hexStr = env->CallObjectMethod(sb, toStrMethodId);
	hexStr = env->PopLocalFrame(hexStr);
	return (jstring)hexStr;
}

jstring md5Digest(JNIEnv* env, jstring input) {
	env->PushLocalFrame(128);
	jstring MD5 = env->NewStringUTF("MD5");
	jclass msgDigestClazz = env->FindClass("java/security/MessageDigest");
	jmethodID getInstanceMethodId = env->GetStaticMethodID(msgDigestClazz, "getInstance",
			"(Ljava/lang/String;)Ljava/security/MessageDigest;");
	jobject msgDigest = env->CallStaticObjectMethod(msgDigestClazz,
			getInstanceMethodId, MD5);

	jclass stringClazz = env->FindClass("java/lang/String");
	jmethodID getBytesMethodId = env->GetMethodID(stringClazz,
			"getBytes", "()[B");
	jbyteArray inputByteArr = (jbyteArray)env->CallObjectMethod(input,
			getBytesMethodId);

	jmethodID updateMethodId = env->GetMethodID(msgDigestClazz,
			"update", "([B)V");
	env->CallVoidMethod(msgDigest, updateMethodId, inputByteArr);

	jmethodID digestMethonId = env->GetMethodID(msgDigestClazz,
			"digest", "()[B");
	jbyteArray digestArr = (jbyteArray)env->CallObjectMethod(msgDigest,
			digestMethonId);

	jobject hexStr = getHexString(env, digestArr, stringClazz);

	hexStr = env->PopLocalFrame(hexStr);
	return (jstring)hexStr;
}

jstring getHexString(JNIEnv* env, jbyteArray inputByteArr, jclass stringClazz) {
	env->PushLocalFrame(128);
	jstring formartStr = env->NewStringUTF("%032x");
	jclass bigIntClazz = env->FindClass("java/math/BigInteger");
	jmethodID initBigIntMethodId = env->GetMethodID(bigIntClazz,
			"<init>", "(I[B)V");

	jobject bigInt = env->NewObject(bigIntClazz, initBigIntMethodId, 1,
			inputByteArr);
	jclass objClazz = env->FindClass("java/lang/Object");

	jobjectArray objArr = env->NewObjectArray(1, objClazz, bigInt);

	jmethodID formatMethodId = env->GetStaticMethodID(stringClazz,
			"format",
			"(Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/String;");
	jobject forward = env->CallStaticObjectMethod(stringClazz,
			formatMethodId, formartStr, objArr);

	forward = env->PopLocalFrame(forward);
	return (jstring)forward;
}

/**
 * 从zip包中提取某个文件
 * apkFilePath: zip文件路径
 * subFile: 需要提取的文件，该文件是zip文件下的子文件
 * dir: 提取文件后生成的路径，subFile生成到该路径下
 */
void unzipFileByName (JNIEnv* env, jstring apkFilePath, const char* subFile,const char* dir)  {
	env->PushLocalFrame(128);
	//将文件路径拼接起来
	int len = strlen(dir) + strlen(subFile) + 1;
	const char * manifestFile = (const char *) malloc(len);
	snprintf((char *) manifestFile, len, "%s%s", dir, subFile);


	jclass fileInputClazz = env->FindClass("java/io/FileInputStream");
	jmethodID fileInputInitMethodID = env->GetMethodID(fileInputClazz,
			"<init>", "(Ljava/lang/String;)V");
	//FileInputStream fis
	jobject fileInputJObject = env->NewObject(fileInputClazz,
			fileInputInitMethodID, apkFilePath);

	jclass zipInputClazz = env->FindClass("java/util/zip/ZipInputStream");
	jmethodID zipInputInitMethodID = env->GetMethodID(zipInputClazz,
			"<init>", "(Ljava/io/InputStream;)V");
	//ZipInputStream zis
	jobject zipInputJObject = env->NewObject(zipInputClazz,
			zipInputInitMethodID, fileInputJObject);
	//getNextEntry
	jmethodID nextEntryMethodID = env->GetMethodID(zipInputClazz,
			"getNextEntry", "()Ljava/util/zip/ZipEntry;");
	//ZipEntry ze
	jobject zipEntryJObject = env->CallObjectMethod(zipInputJObject,
			nextEntryMethodID);
	jclass zipEntryClazz = env->FindClass("java/util/zip/ZipEntry");

	jmethodID getNameMethodID = env->GetMethodID(zipEntryClazz,
			"getName", "()Ljava/lang/String;");

	while (zipEntryJObject != NULL) {
		jobject jname = env->CallObjectMethod(zipEntryJObject,
				getNameMethodID);
		const char* name = env->GetStringUTFChars((jstring)jname, JNI_FALSE);
		if (strcmp(name, subFile) != 0) {
			env->DeleteLocalRef(zipEntryJObject);
			zipEntryJObject = env->CallObjectMethod(zipInputJObject,
					nextEntryMethodID);
			continue;
		}

		env->ReleaseStringUTFChars((jstring)jname, name);
		env->DeleteLocalRef(jname);

//		FILE* outfile = fopen(manifestFile, "wb");
//		if (outfile == NULL) {
//			return;
//		}

		jmethodID readMethodID = env->GetMethodID(zipInputClazz, "read",
				"([B)I");
		jbyteArray jarray = env->NewByteArray(1024);
		int byteLen = 0;
		void* buff = (void*) malloc(1024);

		/******************************现在暂时用Java实现文件读写，以后改成用Java读，C写*********************************/
		jstring manifest = env->NewStringUTF(manifestFile);
		jclass fileOutputClazz = env->FindClass("java/io/FileOutputStream");
		jmethodID fileOutputInitMethodID = env->GetMethodID(fileOutputClazz,
					"<init>", "(Ljava/lang/String;)V");
		//FileOutputStream fos;
		jobject fileOutputJObject = env->NewObject(fileOutputClazz,
				fileOutputInitMethodID, manifest);

		jmethodID writeMethodID = env->GetMethodID(fileOutputClazz, "write", "([BII)V");

		/*******************************现在暂时用Java实现文件读写，以后改成用Java读，C写********************************/
		while ((byteLen = env->CallIntMethod(zipInputJObject, readMethodID,
				jarray)) > 0) {
			env->CallVoidMethod(fileOutputJObject, writeMethodID, jarray, 0, byteLen);
		}
		//close

		jmethodID oupputCloseMethodID = env->GetMethodID(fileOutputClazz, "close",
						"()V");
		env->CallVoidMethod(fileOutputJObject, oupputCloseMethodID);


//		free(outfile);
		env->DeleteLocalRef(jarray);
		env->DeleteLocalRef(manifest);
		env->DeleteLocalRef(fileOutputClazz);
		env->DeleteLocalRef(fileOutputJObject);
		break;
	}

	jmethodID closeMethodID = env->GetMethodID(zipInputClazz, "close",
			"()V");
	env->CallVoidMethod(zipInputJObject, closeMethodID);
	//检测AndroidMinifest.xml的md5
	checkAndroidManifestMd5(manifestFile);

	//清除当前抛出的任何异常。如果当前无异常，则此例程不产生任何效果。
	env->ExceptionClear();
	env->PopLocalFrame(NULL);
}

/**
 * 根据文件路径和md5的长度（16、32位）获取相应的文件md5值
 * path: 文件路径
 * md5_len: md5的长度，有16位和32位
 */
char *MD5_file(const char *path, int md5_len) {
	FILE *fp = fopen(path, "rb");
	MD5_CTX mdContext;
	int bytes;
	unsigned char data[1024];
	char *file_md5;
	int i;
	if (fp == NULL) {
		fprintf(stderr, "fopen %s failed\n", path);
		return NULL;
	}
	MD5Init(&mdContext);
	while ((bytes = fread(data, 1, 1024, fp)) != 0) {
		MD5Update(&mdContext, data, bytes);
	}
	MD5Final(&mdContext);

	file_md5 = (char *) malloc((md5_len + 1) * sizeof(char));
	if (file_md5 == NULL) {
		fprintf(stderr, "malloc failed.\n");
		return NULL;
	}
	memset(file_md5, 0, (md5_len + 1));

	if (md5_len == 16) {
		for (i = 4; i < 12; i++) {
			sprintf(&file_md5[(i - 4) * 2], "%02x", mdContext.digest[i]);
		}
	} else if (md5_len == 32) {
		for (i = 0; i < 16; i++) {
			sprintf(&file_md5[i * 2], "%02x", mdContext.digest[i]);
		}
	} else {
		fclose(fp);
		free(file_md5);
		return NULL;
	}

	fclose(fp);
	return file_md5;
}

