package com.loopher.bugly;

import android.annotation.SuppressLint;
import android.app.Application;
import android.content.Context;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import com.tencent.bugly.beta.Beta;
import com.tencent.tinker.lib.library.TinkerLoadLibrary;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
//    static {
//        System.loadLibrary("native-lib");
//    }

//    static {
//        Log.d("Loopher","MainActivity static method call ===================>");
//        Beta.loadLibrary("native-lib");
//    }


    @SuppressLint("SetTextI18n")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Log.d("Loopher","MainActivity onCreate ==================>");
        // Example of a call to a native method
        loadLibrary();
        TextView tv = (TextView) findViewById(R.id.sample_text);
//        tv.setText(stringFromJNI());
        tv.setText("update UI  text content  by bugly platform ， 计算结果：  value = "+sum(10,20));
//        Log.d("Loopher","adder add() ="+Adder.add(1,12));
        showToast();
        setContext(getApplicationContext());
    }

    private void showToast(){
        Toast.makeText(getApplicationContext(),"这是一个热更新的提示 !"+stringFromJNI() +" "+Adder.add(1,12),Toast.LENGTH_SHORT).show();
    }


    private void loadLibrary(){
        NativeHelper.loadLibrary(getApplicationContext());
     //   Beta.loadArmLibrary(getApplicationContext(),"native-lib");//加载armeabi
//        Beta.loadLibraryFromTinker(getApplicationContext(),"lib/x86","native-lib");//加载x86
       // Beta.loadArmV7Library(getApplicationContext(),"native-lib");//加载armeabi-v7a
        //不能调用tinker的api
//        TinkerLoadLibrary.installNavitveLibraryABI(getApplicationContext(),"armeabi");
//        TinkerLoadLibrary.loadLibraryFromTinker(getApplicationContext(),"lib/armeabi","native-lib");
        Log.d("Loopher","load lib ok ");
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native  int sum(int a,int b);

    public native void setContext(Context context);

}
