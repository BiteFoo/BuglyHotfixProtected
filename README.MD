#2018-01-30
version1.0
使用bugly做热更新测试，目前最新的版本1.1.1，官方最新版
打包命令：
gradlew assembleRelease / assembleDebug
差分包：
gradlew buildTinkerPatchRelease / buildTinkerPatchDebug
主要测试几个功能：
1、如何调用so文件，不是系统调用
根据官方提供的例子MainActivity中提示，"如果想更新so，可以将System.loadLibrary替换成Beta.loadLibrary"
这里不用管是什么abi的，调用方式

 Beta.loadLibrary("mylib");==这种方式和System.loadLibrary("libname");一样
 
 也可以使用如下方式加载so
  Beta.loadArmLibrary(getApplicationContext(),"native-lib"); 加载armeabi
  Beta.loadLibraryFromTinker(getApplicationContext(),"lib/x86","native-lib");//加载x86
  Beta.loadArmV7Library(getApplicationContext(),"native-lib");//加载armeabi-v7a
 
 这里不需要context对象，可以在Bugly初始化完成后，调用
2、如何调用差分文件
Beta.applyTinkerPatch(getApplicationContext(), Environment.getExternalStorageDirectory().getAbsolutePath() + "/patch_signed_7zip.apk");
