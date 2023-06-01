package com.sawyer.studyjni;

import android.util.Log;

public class Dog {

    public Dog() {
        Log.i("lee","(Dog_init)");
    }

    public Dog(int num) {
        Log.i("lee","(Dog_init) num = " + num);
    }

    public Dog(int num1, int num2) {
        Log.i("lee","(Dog_init) num1 = " + num1 + ",num2 = " + num2);
    }

}
