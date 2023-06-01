package com.sawyer.studyjni;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Looper;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import com.sawyer.studyjni.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    //静态代码块在类创建的时候会执行一次
    static {
        //此行代码执行时会调用 JNI_OnLoad(),可用于JNI动态注册时使用
        System.loadLibrary("study_jni");
    }

    private ActivityMainBinding binding;
    private String name = "lee";
    public static int age = 12;
    public final double num = 1.234;
    private int add(int num1, int num2){
        return num1 + num2;
    }

    //C++ 调用 Java的函数
    private String showString(String str,int value){
        Log.d("lee","Java函数被C++调用，str = " + str + ",value = " + value);
        return str + "_" + value;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());

        Log.d("lee","修改前name = " + name);
        changeName();
        Log.d("lee","修改后name = " + name);

        Log.d("lee","修改前age = " + age);
        changeAge();
        Log.d("lee","修改后age = " + age);

        Log.d("lee","修改前num = " + num);
        changeNum();
        Log.d("lee","修改后num = " + num);

        callAddMethod();

        binding.btn1.setOnClickListener(v -> {
            int[] intArray = {1,2,3,4,5,6};
            String[] strArray = {"李小龙","李连杰","李元霸"};
            testArrayAction(intArray,strArray);
            for (int intNum : intArray) {
                Log.i("lee","Java_IntArray_Item:" + intNum);
            }
            for (String str : strArray) {
                Log.i("lee","Java_StringArray_Item:" + str);
            }
        });
        binding.btn2.setOnClickListener(v -> {
            Student stu = new Student();
            stu.setName("lee");
            stu.setAge(12);
            putStudent(stu,"game");
        });
        binding.btn3.setOnClickListener(v -> {
            insertObject();
        });
        binding.btn4.setOnClickListener(v -> {
            testQuote();
        });
        binding.btn5.setOnClickListener(v -> {
            deleteQuote();
        });
        binding.btn6.setOnClickListener(v -> {
            dynamicJavaMethod01();
        });
        binding.btn7.setOnClickListener(v -> {
            int result = dynamicJavaMethod02("单走一张?");
            Toast.makeText(this, "result = " + result, Toast.LENGTH_SHORT).show();
        });
        binding.btn8.setOnClickListener(v -> {
            nativeThread();
        });
        binding.btn9.setOnClickListener(v -> {
            nativeFun1(); //main线程调用
            nativeFun2(); //main线程调用
            staticFun3(); //main线程调用
            new Thread(){ //java子线程调用
                @Override
                public void run() {
                    super.run();
                    staticFun4();
                }
            }.start();
            new Thread(){ //java子线程调用
                @Override
                public void run() {
                    super.run();
                    staticFun4();
                }
            }.start();
        });
        binding.btn10.setOnClickListener(v -> {
            startActivity(new Intent(this,SecondActivity.class));
        });
    }

    public native String stringFromJNI(); // 默认的写法，属于静态注册
    public native void changeName();
    public static native void changeAge();
    public native void changeNum();
    public native void callAddMethod();
    //传递数组
    public native void testArrayAction(int[] intArray, String[] strArray);
    //传递对象、引用类型
    public native void putStudent(Student student, String str);
    //C++ 创建 Java对象
    public native void insertObject();
    //测试C引用
    public native void testQuote();
    //释放C全局引用
    public native void deleteQuote();

    /**
     *JNI函数注册方式：
     *   1.静态注册：默认情况下的native函数就属于静态注册
     * 	   优点：开发简单
     * 	   缺点：1.JNI_native函数名很长
     * 		    2.捆绑了上层，暴露了包名、类型的信息
     * 		    3.在程序运行期间才会去匹配JNI函数，性能稍微弱于动态注册
     *
     *   2.动态注册：Android系统源码里面大量使用了动态注册JNI函数
     *      优点：更安全，因为不会暴露包名、类型
     *
     *     需要借助api: jint JNI_OnLoad(JavaVM * javaVm, void * args) 来完成
     *     上边的静态代码块中，加载xxx.so库时，会调用此函数
     *
     *     性能比静态注册快一点的原因：
     *          在此静态代码块中提前注册好了，而静态注册则在运行期间才去匹配
     *          static {
     *              System.loadLibrary("studyjni");
     *          }
     */
    public native void dynamicJavaMethod01(); //动态注册
    public native int dynamicJavaMethod02(String str); //动态注册

    //JNI线程
    public native void nativeThread(); // Java层调用Native层的函数，完成JNI线程
    public native void closeThread();  // 释放全局引用
    //todo =============下面是 被native代码调用的 Java方法=========
    public void updateActivityUI() {
        if (Looper.getMainLooper() == Looper.myLooper()) { // todo 代表C++用主线程调用此函数
            new AlertDialog.Builder(MainActivity.this)
                    .setTitle("UI")
                    .setMessage("updateActivityUI Activity UI ...")
                    .setPositiveButton("老夫知道了", null)
                    .show();
        } else {    // todo 代表C++用异步线程调用此函数
            Log.d("lee", "updateActivityUI 所属于子线程，只能打印日志了..");
            runOnUiThread(new Runnable() { // 哪怕是异步线程
                @Override
                public void run() {
                    // 可以在子线程里面 操作UI
                    new AlertDialog.Builder(MainActivity.this)
                            .setTitle("updateActivityUI")
                            .setMessage("所属于子线程，只能打印日志了..")
                            .setPositiveButton("老夫知道了", null)
                            .show();
                }
            });
        }
    }

    //todo =================JavaVM、JNIEnv作用域范围===================
    public native void nativeFun1();
    public native void nativeFun2();
    public static native void staticFun3();
    public static native void staticFun4();


    @Override
    protected void onDestroy() {
        super.onDestroy();
        deleteQuote();
        closeThread();
    }
}
