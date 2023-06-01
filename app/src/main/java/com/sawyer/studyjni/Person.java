package com.sawyer.studyjni;

import android.util.Log;

public class Person {

    public Student student;

    public void setStudent(Student stu){
        Log.i("lee", "Java_setStudent_stu = " + stu.toString());
        this.student = stu;
    }

    public static void putStudent(Student stu){
        Log.i("lee", "Java_static_putStudent_stu = " + stu.toString());
    }
}
