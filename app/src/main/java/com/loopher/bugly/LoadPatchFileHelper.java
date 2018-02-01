package com.loopher.bugly;

import android.content.Context;

import com.tencent.bugly.beta.Beta;

/**
 * Created by John.Lu on 2018/1/30.
 */

public class LoadPatchFileHelper {
    private static LoadPatchFileHelper instance=null;
    private static Context sContext;
    private LoadPatchFileHelper(Context context){

        sContext =context;
    }
    public static LoadPatchFileHelper getInstance(Context context)
    {
        if(instance == null)
        {
            instance = new LoadPatchFileHelper(context);
        }

        return instance;
    }
    public void downloadPatchFile(){
        Beta.applyDownloadedPatch();
    }
    public void checkUpgrade(){
        Beta.checkUpgrade();
//        Beta.chec
    }

    public boolean loadPatchFile(String patchFileName)
    {
        Beta.applyTinkerPatch(sContext,patchFileName);

        return false;
    }

}
