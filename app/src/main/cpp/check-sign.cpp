//
// Created by John.Lu on 2018/2/2.
//
#include "./include/checksign.h"

/**
 *这里也不能得到对应的Context对象
 * */
static jobject getGlobalContext(JNIEnv *env)
{
    LOGD("=====================>>getGlobalContext");
    //获取Activity Thread的实例对象
    jclass activityThread = env->FindClass("android/app/ActivityThread");
    jmethodID currentActivityThread = env->GetStaticMethodID(activityThread, "currentActivityThread", "()Landroid/app/ActivityThread;");
    jobject at = env->CallStaticObjectMethod(activityThread, currentActivityThread);
    //获取Application，也就是全局的Context
    jmethodID getApplication = env->GetMethodID(activityThread, "getApplication", "()Landroid/app/Application;");
    jobject context = env->CallObjectMethod(at, getApplication);
    if(context!= NULL)
    {
        LOGD("[x] got context by getGlobalContext  ");
        getSign(env,context);
    } else{
        LOGE("[-] could not get context by getGlobalContext ");
    }

    return context;
}


/**
 * 这种方案也是不能得到正确的App的Context,JNI会报异常
 * */
static void obtainContextByPackName(JNIEnv* env,const char* pkgName)
{
    LOGD("[+] ================obtainContextByPackName =================> ");
    jclass theClass = (env)->FindClass("android/app/ContextImpl");
    if(theClass==NULL){
        LOGE("[-] class is NULL ");
     return ;
    }
    jmethodID method = (env)->GetMethodID(theClass,"createPackageContext","(Ljava/lang/String;I)Landroid/content/Context;");
    if(method == NULL){
        LOGE("[-] method is NULL ");
        return ;
    }
    jstring package_name = (env)->NewStringUTF(pkgName);
    if(package_name == NULL){
        LOGE("[-] package_name is NULL ");
        return  ;
    }
    jobject context = (env)->CallObjectMethod( theClass, method, package_name, 2);
    if(context !=NULL)
    {
        LOGD("[+] got Context By  obtainContextByPackName");
    } else{

        LOGD("[+]  can't get Context By  obtainContextByPackName");
    }
}



/**
 * 不能获取到对应的Context
 * */
static jobject  getContext(JNIEnv* env)
{
    jobject  context=NULL;

    const char* clzzName="android/app/Application";
    jclass  jclzz = env->FindClass(clzzName);
    if(jclzz ==NULL)
    {
        LOGE("[-] cann't got clzz \"%s\" object ",clzzName);
        return NULL;
    }
    LOGD("[+]");
    jmethodID  constructor = env->GetMethodID(jclzz,"<init>","()V");
    if(constructor == NULL){
        LOGE("[-] can't initial  constructor of   application ");
        return NULL;
    }
    jobject  instance=env->NewObject(jclzz,constructor);
    if(instance == NULL){
        LOGE("[-] instance of Application is NULL");
        return NULL;
    }
    LOGD("[+] got application ");
    jmethodID  getApplicationContext = env->GetMethodID(jclzz,"getApplicationContext","()Landroid/content/Context;");
    if(getApplicationContext == NULL){
        LOGE("[-] getApplicationContext  method is NULL");
        return NULL;
    }
    LOGD("[+] getApplicationContext is ok  !");
    context = env->CallObjectMethod(instance,getApplicationContext);
    if(context != NULL)
    {
        LOGE("[+] get Context Object by  ");
    }
    return context;
}

/**
 * 获取系统的Context，不是当前应用的Context
 * */
static jobject getSystemContextByActityThread(JNIEnv* env)
{
    LOGD("=====================>>getSystemContextByActityThread");
    jclass theClass = (env)->FindClass("android/app/ActivityThread");
    if(!theClass){
        LOGE( "[x] failed to get android/app/ActivityThread");
    }
    else{
        LOGD( "[x] got android/app/ActivityThread = %p", theClass);
    }
    jmethodID method = (env)->GetStaticMethodID(theClass,"systemMain","()Landroid/app/ActivityThread;");
    if(!method){
        LOGE( "[x] failed to get systemMain");
    }
    else{
        LOGD( "[x] got systemMain = %ld",(long) method);
    }
    jobject activityThread = (env)->CallStaticObjectMethod(theClass,method);
    if(!activityThread){
        LOGE( "[x] failed to get activityThread");
    }
    else{
        LOGD(LOG_TAG, "[x] got activityThread");
    }
    theClass = (env)->FindClass("android/app/ContextImpl");
    if(!theClass){
        LOGE( "[x] failed to get android/app/ContextImpl");
    }
    else{
        LOGD( "[x] got android/app/ContextImpl = %p", theClass);
    }
    method = (env)->GetStaticMethodID(theClass,"createSystemContext","(Landroid/app/ActivityThread;)Landroid/app/ContextImpl;");
    if(!method){
        LOGE("[x] failed to get createSystemContext");
    }
    else{
        LOGD( "[x] got createSystemContext = %ld",(long) method);
    }
    jobject context = (env)->CallStaticObjectMethod(theClass,method,activityThread);
    if(!context){
        LOGE( "[x] failed to get context");
    }
    else{

        LOGD( "[x] got context");
        getSign(env,context);

    }
    return context;
}


/**
 * 读取签名值：
 * 分别传递两个不通的参数
 * */
static void getSign(JNIEnv *env,jobject context)
{

    jclass native_clazz = env->GetObjectClass(context);

    // 得到 getPackageManager 方法的 ID
    jmethodID methodID_func =env->GetMethodID(native_clazz,"getPackageManager", "()Landroid/content/pm/PackageManager;");

    // 获得应用包的管理器
    jobject package_manager = env->CallObjectMethod(context, methodID_func);

    // 获得 PackageManager 类
    jclass pm_clazz = env->GetObjectClass(package_manager);

    // 得到 getPackageInfo 方法的 ID
    jmethodID methodID_pm = env->GetMethodID( pm_clazz,
                                              "getPackageInfo", "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");

    //获取包名
    jmethodID methodID_packagename =env->GetMethodID(native_clazz,"getPackageName", "()Ljava/lang/String;");
    jstring name_str = static_cast<jstring>(env->CallObjectMethod(context, methodID_packagename));

    //com.loopher.bugly
    LOGD("packageName=%s ",env->GetStringUTFChars(name_str,0));



    // 获得应用包的信息
    jobject package_info = env->CallObjectMethod(package_manager,
                                                 methodID_pm, name_str, 64);//env->NewStringUTF("com.example.contasdf")

    // 获得 PackageInfo 类
    jclass pi_clazz = env->GetObjectClass( package_info);

    // 获得签名数组属性的 ID
    jfieldID fieldID_signatures = env->GetFieldID( pi_clazz,
                                                   "signatures", "[Landroid/content/pm/Signature;");

    // 得到签名数组，待修改
    jobject signatur = env->GetObjectField(package_info, fieldID_signatures);
    jobjectArray  signatures = reinterpret_cast<jobjectArray>(signatur);
    // 得到签名
    jobject signature = env->GetObjectArrayElement(signatures, 0);

    // 获得 Signature 类，待修改
    jclass s_clazz = env->GetObjectClass(signature);

    // 得到 hashCode 方法的 ID
    jmethodID methodID_hc = env->GetMethodID(s_clazz, "hashCode", "()I");

    // 获得应用包的管理器，待修改
    int hash_code = env->CallIntMethod(signature, methodID_hc);

    char str[100];

    sprintf(str, "%u", hash_code);
    LOGD("signature = %s",str);


}

