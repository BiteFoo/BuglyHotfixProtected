package com.loopher.bugly;

import android.app.Application;
import android.content.Context;
import android.content.Intent;
import android.os.Environment;
import android.support.multidex.MultiDex;
import android.widget.Toast;

import com.tencent.bugly.Bugly;
import com.tencent.bugly.beta.Beta;
import com.tencent.bugly.beta.interfaces.BetaPatchListener;
import com.tencent.tinker.loader.app.DefaultApplicationLike;

import java.io.File;
import java.util.Locale;

/**
 * Created by John.Lu on 2018/1/30.
 */

public class MyApplicationLike extends DefaultApplicationLike {
    public MyApplicationLike(Application application,
                             int tinkerFlags,
                             boolean tinkerLoadVerifyFlag,
                             long applicationStartElapsedTime,
                             long applicationStartMillisTime,
                             Intent tinkerResultIntent) {
        super(application, tinkerFlags, tinkerLoadVerifyFlag, applicationStartElapsedTime, applicationStartMillisTime, tinkerResultIntent);
    }

    @Override
    public void onBaseContextAttached(Context base) {
        super.onBaseContextAttached(base);
        MultiDex.install(base);
        Beta.installTinker(this);
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Beta.enableHotfix =true;
        Beta.canAutoDownloadPatch =true;
        Beta.canAutoPatch =true;
        Beta.canNotifyUserRestart =true;
        Beta.betaPatchListener = new BetaPatchListener() {
            @Override
            public void onPatchReceived(String patchFile) {
                Toast.makeText(getApplication(), "补丁下载地址" + patchFile, Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onDownloadReceived(long savedLength, long totalLength) {
                Toast.makeText(getApplication(),
                        String.format(Locale.getDefault(), "%s %d%%",
                                Beta.strNotificationDownloading,
                                (int) (totalLength == 0 ? 0 : savedLength * 100 / totalLength)),
                        Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onDownloadSuccess(String msg) {
                Toast.makeText(getApplication(), "补丁下载成功", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onDownloadFailure(String msg) {
                Toast.makeText(getApplication(), "补丁下载失败", Toast.LENGTH_SHORT).show();

            }

            @Override
            public void onApplySuccess(String msg) {
                Toast.makeText(getApplication(), "补丁应用成功", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onApplyFailure(String msg) {
                Toast.makeText(getApplication(), "补丁应用失败", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onPatchRollback() {

            }
        };

        // 设置开发设备，默认为false，上传补丁如果下发范围指定为“开发设备”，需要调用此接口来标识开发设备
        Bugly.setIsDevelopmentDevice(getApplication(), true);
        // 多渠道需求塞入
        // String channel = WalleChannelReader.getChannel(getApplication());
        // Bugly.setAppChannel(getApplication(), channel);
        // 这里实现SDK初始化，appId替换成你的在Bugly平台申请的appId
        Bugly.init(getApplication(), "346fb0ed2c", true);
       // initPatchFileHelper();

    }
    /**
     * 初始化bugly的相关操作
     * 这里不需要再主动调用，bugly已经通过全局变量自动完成
     * */
    private void initPatchFileHelper(){
        String file = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator+Const.PATCH_FILE_NAME;
        LoadPatchFileHelper helper = LoadPatchFileHelper.getInstance(getApplication());
        helper.checkUpgrade();
        helper.downloadPatchFile();
        helper.loadPatchFile(file);
    }

    @Override
    public void onTerminate() {
        super.onTerminate();
        Beta.unInit();
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
    }
}
