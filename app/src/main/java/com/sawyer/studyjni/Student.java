package com.sawyer.studyjni;

import android.util.Log;

import androidx.annotation.NonNull;

public class Student {

    public String name;
    public int age;


    public String getName() {
        return name;
    }

    public void setName(String name) {
        Log.i("lee","Java_SetName_name = " + name);
        this.name = name;
    }

    public int getAge() {
        return age;
    }

    public void setAge(int age) {
        Log.i("lee","Java_SetAge_age = " + age);
        this.age = age;
    }

    public static void showInfo(String info) {
        Log.i("lee","Java_showInfo_str = " + info);
    }

    @NonNull
    @Override
    public String toString() {
        return "Student{" +
                "name='" + name + '\'' +
                ", age=" + age +
                '}';
    }
}
