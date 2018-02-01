package com.loopher.bugly;

import android.content.Context;
import android.text.TextUtils;
import android.util.Log;

import com.tencent.bugly.beta.Beta;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

/**
 * Created by John.Lu on 2018/1/30.
 */

public class NativeHelper {


    private static String getArch()
    {
        try {
            String abi = new BufferedReader(new InputStreamReader(Runtime.getRuntime().exec("getprop ro.product.cpu.abi").getInputStream())).readLine();
            if (!TextUtils.isEmpty(abi) && abi.contains("x86"))
            {
                return "x86";
            }
        } catch (IOException e) {
            e.printStackTrace();
            return "arm";  //异常情况，默认返回arm架构
        }

        return "arm";
    }
    public static void loadLibrary(Context context)
    {
        if("x86".equals(getArch()))
        {
            Log.d("Loopher","=========load x86========>");
            Beta.loadLibraryFromTinker(context,"lib/x86","native-lib");//加载x86
        }
        else
        {


            Log.d("Loopher","=========load armeabi========>");
            Beta.loadArmLibrary(context,"native-lib");//加载armeabi
        }

        //   Beta.loadArmLibrary(getApplicationContext(),"native-lib");//加载armeabi

        // Beta.loadArmV7Library(getApplicationContext(),"native-lib");//加载armeabi-v7a
        //不能调用tinker的api
//        TinkerLoadLibrary.installNavitveLibraryABI(getApplicationContext(),"armeabi");
//        TinkerLoadLibrary.loadLibraryFromTinker(getApplicationContext(),"lib/armeabi","native-lib");
    }
}
