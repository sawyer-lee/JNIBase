#include <jni.h>
#include <string>   //C++导入头文件的写法
//#include <string.h>  C导入头文件的写法

//日志输出
#include <android/log.h>
#define TAG "lee"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__);
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__);
#include <pthread.h> // 在AS上pthread不需要额外配置，默认就有

/**
 * 修饰符：
 *      @extern "C"：表示下面的代码，都采用C的编译方式
 *      @JNIEXPORT： JNI标记关键字，标记该函数可被外界调用，不能缺少
 *      @jstring：函数返回值，对应java方法的String
 *      @JNICALL：也是一个关键字(可缺少)，用来约束函数入栈顺序和堆栈的内存清理规则
 *
 * 函数命名规则：Java_com_sawyer_studyjni_MainActivity_stringFromJNI
 *      即：Java_包名_类名_函数名。若Java里面包含"_"，需在_后面加一个1
 * 示例：包名为com.sawyer.study_jni，则JNI中函数名为Java_com_sawyer_study_1jni_MainActivity_stringFromJNI
 *
 * 参数：
 *      @JNIEnv *: 为Java和C++交互的桥梁。 掌握其内置的function，就可以掌握JNI技术了
 *      @jobject: java层代码传递过来的对象，即本项目中的 MainActivity 对象。对应于java中该函数为非静态
 *      @jclass: java层代码传递过来的Class对象，即本项目中的 MainActivity.class 对象。对应于java中该函数为静态
 */
extern "C" JNIEXPORT jstring JNICALL
Java_com_sawyer_studyjni_MainActivity_stringFromJNI(JNIEnv* env,jobject mainActivityThis){
    std::string hello = "sawyer say hello from C++ ";
    /**
     * 重要：
     * JNIEnv * 的不同：（原因；可点进去查看其实现方式，即jni.h文件里面的实现）
     * (1)当前文件格式为C,即native-lib.c
     *      env：它其实是二级指针，而 -> 是一级指针的调用方式，所以调用函数时，写法为：
     *      return (*env)->NewStringUTF(env,hello.c_str());
     * (2)当前文件格式为C++,即native-lib.cpp
     *      env：它其实是一级指针，所以调用函数时，写法为：
     *      return env->NewStringUTF(hello.c_str());
     */
    return env->NewStringUTF(hello.c_str());
}

//函数示例：修改MainActivity中的非静态变量name
extern "C" JNIEXPORT void JNICALL
Java_com_sawyer_studyjni_MainActivity_changeName(JNIEnv *env, jobject mainActivityThis) {
    //api: jclass GetObjectClass(jobject obj)
    jclass mainActivityClass = env -> GetObjectClass(mainActivityThis);

    /**
     * api: jfieldID GetFieldID(jclass clazz, const char* name, const char* sig)
     * 参数:
     *      @name: MainActivity中的变量名
     *      @sig: 签名，即C++调用Java，因为它们的数据类型不一样，所以需要签名规则，也用来避免java的重载函数
     *      规则如下：
     *          Java类型------对应的属性类型符号
     *              boolean------Z
     *              byte------B
     *              char------C
     *              short------S
     *              int------I
     *              long------J
     *              float------F
     *              double------D
     *              void------V
     *              object------L全类名
     *              int[]------[I  签名规则为：左中括号[ + 对象或者基本类型的签名
     *              函数------(参数类型)返回值类型   e.g：void add(int num1,double num2)---(ID)V
     *
     */
    jfieldID nameFid = env -> GetFieldID(mainActivityClass,"name","Ljava/lang/String;");

    jstring value = env -> NewStringUTF("sawyer");

    //api: void SetObjectField(jobject obj, jfieldID fieldID, jobject value)
    env -> SetObjectField(mainActivityThis, nameFid, value);
}

//函数示例：修改MainActivity中的静态变量age
extern "C" JNIEXPORT void JNICALL
Java_com_sawyer_studyjni_MainActivity_changeAge(JNIEnv *env, jclass mainActivityClass) {
    //api: jfieldID GetStaticFieldID(jclass clazz, const char* name, const char* sig)
    jfieldID ageFid = env -> GetStaticFieldID(mainActivityClass,"age","I");

    /** 修改方式一 */
    //jint GetStaticIntField(jclass clazz, jfieldID fieldID)
    int age = env -> GetStaticIntField(mainActivityClass,ageFid);
    //api: void SetStaticIntField(jclass clazz, jfieldID fieldID, jint value)
    env -> SetStaticIntField(mainActivityClass, ageFid,age + 1);

    /** 修改方式二 */
    //env -> SetStaticIntField(mainActivityClass, ageFid,14);
}

//函数示例：修改MainActivity中的final变量num
extern "C" JNIEXPORT void JNICALL
Java_com_sawyer_studyjni_MainActivity_changeNum(JNIEnv *env, jobject mainActivityThis) {
    jclass mainActivityClass = env -> GetObjectClass(mainActivityThis);
    jfieldID numFid = env -> GetFieldID(mainActivityClass,"num","D");
    env -> SetDoubleField(mainActivityThis, numFid,99.999);

    /** JNI中打印 */
    double result = env -> GetDoubleField(mainActivityThis,numFid);
    LOGD("C++修改final num的result = %lf\n",result)
}

//函数示例：调用MainActivity中的函数
extern "C" JNIEXPORT void JNICALL
Java_com_sawyer_studyjni_MainActivity_callAddMethod(JNIEnv *env, jobject mainActivityThis) {
    jclass mainActivityClass = env -> GetObjectClass(mainActivityThis);
    jmethodID methodId = env -> GetMethodID(mainActivityClass,"add","(II)I");
    int result = env -> CallIntMethod(mainActivityThis,methodId,2,6);
    LOGD("add_result = %d\n",result)

    //todo 调用main中的showString()函数
    jmethodID showStringMid = env -> GetMethodID(mainActivityClass,"showString", "(Ljava/lang/String;I)Ljava/lang/String;");
    jstring value = env -> NewStringUTF("逅lee懈");
    //api: jobject   (*CallObjectMethod)(JNIEnv*, jobject, jmethodID, ...);  //...即多个参数
    //class _jstring : public _jobject {};  继承关系
    auto resultStr = (jstring)env -> CallObjectMethod(mainActivityThis, showStringMid, value, 9527);
    const char * resultCharStr = env -> GetStringUTFChars(resultStr,NULL);
    LOGD("C++_showString_result = %s", resultCharStr)
    env->ReleaseStringUTFChars(resultStr,resultCharStr);
}

//函数示例：JNI数组操作
extern "C" JNIEXPORT void JNICALL
Java_com_sawyer_studyjni_MainActivity_testArrayAction(JNIEnv *env,
                                                      jobject mainActivityThis,
                                                      jintArray int_array,
                                                      jobjectArray str_array) {
    //遍历Int[]，即基本数据类型数组
    //api：GetArrayLength  获取数组的长度
    int intArrayLen = env->GetArrayLength(int_array);
    for (int i = 0; i < intArrayLen; ++i) {
        //参数二：NULL在C中就是0。而在C中，非0即true，即0 == false。此处传入false即不进行copy机制
        jint * _intItem = env->GetIntArrayElements(int_array,NULL);

        *(_intItem + i) = i + 10001;
        LOGD("C++_IntArray_Item: %d\n" , *(_intItem + i))

        /**
         * C语言很重要的一点：释放工作，一定要做
         *
         * api：void ReleaseIntArrayElements(jintArray array, jint* elems,jint mode)
         *   @mode: (操纵杆 == JNIEnv)
         *      JNI_OK: 先用操纵杆将数据刷新到JVM，再释放C++数组
         *      JNI_COMMIT: 仅使用操纵杆刷新数据到JVM
         *      JNI_ABORT: 仅释放C++数组
         *
         * 上面这行代码 *(_intItem + i) = i + 10001; 修改了C++中的值，
         * 并且使用了mode = JNI_OK，故会将修改的值传给Java层
         */
        env->ReleaseIntArrayElements(int_array, _intItem, JNI_OK);
    }

    //遍历String[]，即引用类型数组
    int strArrayLen = env->GetArrayLength(str_array);
    for (int i = 0; i < strArrayLen; ++i) {
        auto strItem = (jstring)env->GetObjectArrayElement(str_array, i);
        const char * _str_item = env->GetStringUTFChars(strItem, NULL);
        LOGD("修改前：C++_strArray_Item = %s",_str_item)
        //todo JNI函数使用很重要的一点：释放工作，一定要做，这样才专业
        env->ReleaseStringUTFChars(strItem, _str_item);

        /** 对引用数组的元素进行修改 */
        jstring _value = env->NewStringUTF("hello item");
        env->SetObjectArrayElement(str_array, i , _value);
        /** 获取修改后的元素并进行打印 */
        auto strItem2 = (jstring)env->GetObjectArrayElement(str_array , i);
        const char * _str_item2 = env->GetStringUTFChars(strItem2, NULL);
        LOGD("修改后：C++_strArray_Item = %s",_str_item2)
        //todo JNI函数使用很重要的一点：释放工作，一定要做，这样才专业
        env->ReleaseStringUTFChars(strItem2, _str_item2);
    }

}

//函数示例：JNI对象操作
extern "C" JNIEXPORT void JNICALL
Java_com_sawyer_studyjni_MainActivity_putStudent(JNIEnv *env, jobject mainActivityThis,
                                                 jobject student, jstring str) {
    //先搞定简单的jstring
    const char * _str = env->GetStringUTFChars(str, nullptr);
    LOGD("C++_str = %s",_str)
    //todo JNI函数使用很重要的一点：释放工作，一定要做，这样才专业
    env->ReleaseStringUTFChars(str, _str);

    //todo 记得要释放
    jclass stuClass = env->FindClass("com/sawyer/studyjni/Student");

    //调用Java层的toString()
    jmethodID toStringMid = env -> GetMethodID(stuClass,"toString","()Ljava/lang/String;");
    auto toStringStr = (jstring)env -> CallObjectMethod(student, toStringMid);
    const char * _to_string_char = env -> GetStringUTFChars(toStringStr, nullptr);
    LOGD("C++_toString_str = %s", _to_string_char)
    env->ReleaseStringUTFChars(toStringStr, _to_string_char);

    //调用Java层的setName()
    jmethodID setNameMid = env->GetMethodID(stuClass,"setName","(Ljava/lang/String;)V");
    jstring nameStr = env->NewStringUTF("kobe");
    env->CallVoidMethod(student, setNameMid, nameStr);

    //调用Java层的getName()
    jmethodID getNameMid = env->GetMethodID(stuClass,"getName","()Ljava/lang/String;");
    auto nameStrResult = (jstring)env->CallObjectMethod(student,getNameMid);
    const char * _name_str_result = env->GetStringUTFChars(nameStrResult, nullptr);
    LOGD("C++_getName_str = %s", _name_str_result)
    env->ReleaseStringUTFChars(nameStrResult, _name_str_result);

    //调用Java层的setAge()
    jmethodID setAgeMid = env->GetMethodID(stuClass,"setAge","(I)V");
    env->CallVoidMethod(student, setAgeMid, 41);

    //调用Java层的getAge()
    jmethodID getAgeMid = env->GetMethodID(stuClass,"getAge","()I");
    int ageResult = env->CallIntMethod(student,getAgeMid);
    LOGD("C++_getAge = %d", ageResult)

    //调用Java层的 showInfo()#Student
    jmethodID showInfoMid = env->GetStaticMethodID(stuClass, "showInfo", "(Ljava/lang/String;)V");
    jstring showInfoStr = env->NewStringUTF("像我这样优秀的人");
    env->CallStaticVoidMethod(stuClass, showInfoMid, showInfoStr);

    //todo JNI函数使用很重要的一点：释放工作，一定要做，这样才专业
    // 由于NewStringUTF没有对应的Releasexxx()
    env->DeleteLocalRef(nameStr);
    env->DeleteLocalRef(showInfoStr);
    env->DeleteLocalRef(stuClass);
}
//函数示例：JNI凭空创建Java对象
extern "C" JNIEXPORT void JNICALL
Java_com_sawyer_studyjni_MainActivity_insertObject(JNIEnv *env, jobject mainActivityThis) {

    jclass personClass = env->FindClass("com/sawyer/studyjni/Person");
    jobject personObj = env->AllocObject(personClass);
    //获取student对象
    jclass studentClass = env->FindClass("com/sawyer/studyjni/Student");
    jobject studentObj = env->AllocObject(studentClass);
    //给student对象赋值
    jmethodID setNameMid = env->GetMethodID(studentClass,"setName", "(Ljava/lang/String;)V");
    auto stuName = (jstring) env->NewStringUTF("唐三");
    env->CallVoidMethod(studentObj, setNameMid, stuName);
    jmethodID setAgeMid = env->GetMethodID(studentClass,"setAge", "(I)V");
    env->CallVoidMethod(studentObj, setAgeMid, 100);

    //调用Java Person对象的setStudent()
    jmethodID setStuMid = env->GetMethodID(personClass,"setStudent", "(Lcom/sawyer/studyjni/Student;)V");
    env->CallVoidMethod(personObj, setStuMid, studentObj);

    //调用Java Person对象的putStudent()
    jmethodID putStuMid = env->GetStaticMethodID(personClass,"putStudent","(Lcom/sawyer/studyjni/Student;)V");
    env->CallStaticVoidMethod(personClass, putStuMid,studentObj);

    //todo JNI函数使用很重要的一点：释放工作，一定要做，这样才专业
    //DeleteLocalRef: 释放局部变量
    //DeleteGlobalRef: 释放全局变量
    env->DeleteLocalRef(personClass);
    env->DeleteLocalRef(personObj);
    env->DeleteLocalRef(studentClass);
    env->DeleteLocalRef(studentObj);
    env->DeleteLocalRef(stuName);
}//此函数弹栈后，也会去释放内存，但为什么在上面要去做释放工作呢？
//答：若此函数有成千上万行代码，等到函数弹栈后再释放，就有可能导致内存释放不及时，不够用的情况

jclass dogClass = nullptr;  //此处的dogClass仍然是局部引用

/**
 *函数示例：JNI全局引用、局部引用
 * 默认情况下，是局部引用，在JNI函数执行结束后，会自动回收局部引用。但是，还是要养成时时刻刻及时回收内存的好习惯
 * 全局引用：需要我们手动去提升为全局引用。提升为全局引用后，必须我们自己手动释放。
 * 所以通常情况下，会在Activity的onDestroy()中释放全局引用
 */
extern "C" JNIEXPORT void JNICALL
Java_com_sawyer_studyjni_MainActivity_testQuote(JNIEnv *env, jobject mainActivityThis) {
    if (!dogClass){ // 等价于 dogClass == NULL
        //dogClass = env->FindClass("com/sawyer/studyjni/Dog"); 局部引用

        //提升dogClass为全局引用,JNI函数执行完后不会被释放
        jclass tempDogClass = env->FindClass("com/sawyer/studyjni/Dog");
        dogClass = (jclass) env->NewGlobalRef( (jobject)tempDogClass );//class _jclass : public _jobject {};
        env->DeleteLocalRef(tempDogClass);
    }

    //<init> == 构造函数名
    jmethodID dogMid1 = env->GetMethodID(dogClass, "<init>", "()V");
    //NewObject() == 调用构造函数
    jobject dog1 = env->NewObject(dogClass, dogMid1); //无参构造

    jmethodID dogMid2 = env->GetMethodID(dogClass, "<init>", "(I)V"); //一个参数构造方法
    jobject dog2 = env->NewObject(dogClass, dogMid2, 666);

    jmethodID dogMid3 = env->GetMethodID(dogClass, "<init>", "(II)V"); //两个参数构造方法
    jobject dog3 = env->NewObject(dogClass, dogMid3, 33, 99);
}//若不提升为全局引用，在JNI函数弹栈后，会自动释放局部引用dogClass，但是dogClass不会指向NULL，会指向一个别的系统值。故第二次调用该函数会发生崩溃

//函数示例：JNI释放全局引用
extern "C" JNIEXPORT void JNICALL
Java_com_sawyer_studyjni_MainActivity_deleteQuote(JNIEnv *env, jobject mainActivityThis) {
    if (dogClass) {  // 等价于 dogClass ！= NULL
        env->DeleteGlobalRef(dogClass);
        dogClass = nullptr;  //全局引用被释放后，其指向依然不会指向NULL，多次调用依然会崩溃
        LOGD("成功手动释放全局引用！")
    }else {
        LOGD("全局引用已经被释放了！")
    }
}


//===============================动态注册JNI函数===========================

//提升下变量的作用域
JavaVM* jvm = nullptr;

/**
 *动态注册的JNI函数的实现
 * 方式一：void dynamicJavaMethod01(JNIEnv * env, jobject mainThis)
 * 方式二：void dynamicJavaMethod01()
 * 里面的参数写不写：取决于要不要使用该参数。如果不使用，可以省略
*/
void dynamic_java_method01(){
    LOGD("动态注册的函数，method01在C++中的实现完成了！")
}

jint dynamic_java_method02(JNIEnv * env, jobject mainThis, jstring str){
    const char * str_ = env->GetStringUTFChars(str, nullptr);
    LOGD("Java传递给C的字符串为：%s",str_)
    env->ReleaseStringUTFChars(str,str_);
    return 6;
}

/**
 * jni.h中的定义：
    typedef struct {
        const char* name;       //Java中动态注册的函数名
        const char* signature;  //Java中动态注册的函数的签名
        void* fnPtr;            //函数指针，即C++的函数名
    } JNINativeMethod;
*/
static const JNINativeMethod methods[] = {
        {"dynamicJavaMethod01", "()V", (void *)(dynamic_java_method01)},
        {"dynamicJavaMethod02", "(Ljava/lang/String;)I", (jint *)(dynamic_java_method02)}
};

/**
 *此函数类似Java对象的无参构造函数，不写的话本就存在，写出来会覆盖默认的
 * java层的System.loadLibrary("studyjni");执行时就会调用此函数
 */
jint JNI_OnLoad(JavaVM* javaVm, void* args){
    ::jvm = javaVm;

    JNIEnv* env = nullptr;
    //获取JNIEnv对象
    int result = jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    //Java网络请求result = 200,代表成功
    //C的请求结果result = 0,代表成功
    // if (result) == if (result != JNI_OK)
    if (result != JNI_OK){
        return -1; //故意让程序崩溃
    }

    jclass mainActClass = env->FindClass("com/sawyer/studyjni/MainActivity");
    /**
     * api:jint RegisterNatives(jclass clazz, const JNINativeMethod* methods,jint nMethods)
     * 作用：一次性可动态注册多个JNI函数
     * @jclass: native函数存在于哪个类中
     * @methods: 所有要动态注册的函数，所存放的数组
     * @nMethods: 数组中函数的个数，可以用这样的计算方式：sizeof(methods)/sizeof(JNINativeMethod)
     */
    env->RegisterNatives(mainActClass, methods, sizeof(methods)/sizeof(JNINativeMethod));

    return JNI_VERSION_1_6; //一般会使用最新的版本标记
}

//==================================JNI线程操作===============================
class MyContext {
public:
    //JNIEnv *jniEnv = nullptr; //不能跨线程传递JNIEnv。否则会奔溃
    jobject instance = nullptr; //局部成员不能跨线程、跨函数。否则会奔溃
};

/**
 * pthread_create()的第三个参数,函数指针，相当于Java线程的run函数
 * @args: pthread_create()的第四个参数
 * void * ：相当于Java的object，即可以进行所有的类型转换
 */
void * cpp_thread_run(void * args){
    auto * context = static_cast<MyContext *>(args);
    //从全局成员jvm中获取JNIEnv，需要通过jvm创建当前线程的JNIEnv，即子线程附加自己的JNIEnv
    JNIEnv * asyncEnv ;
    jint result = ::jvm->AttachCurrentThread(reinterpret_cast<JNIEnv **>(&asyncEnv), nullptr);
    if (result){  //C++返回结果为0表示成功，而C++中非0即true
        return nullptr;
    }

    /**
     * todo 前面的代码所做的工作:
     *      1.给子线程附加JNIEnv
     *      2.将MainActivity提升为全局成员
     * 所以当前函数cpp_thread_run()所在的子线程，才可以去调用主线程的函数
     */
    jclass mainClass = asyncEnv->GetObjectClass(context->instance);
    jmethodID nativeThreadMid = asyncEnv->GetMethodID(mainClass,"updateActivityUI","()V");
    asyncEnv->CallVoidMethod(context->instance, nativeThreadMid);

    //解除当前线程所附加的JNIEnv
    ::jvm->DetachCurrentThread();

    return nullptr; //必须要返回0,否则程序会崩溃
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sawyer_studyjni_MainActivity_nativeThread(JNIEnv *env, jobject mainActivityThis) {
    //此函数nativeThread()默认运行在主线程中，即Android UI线程
    /*jclass mainClass = env->GetObjectClass(mainActivityThis);
    jmethodID nativeThreadMid = env->GetMethodID(mainClass,"nativeThread","()V");
    env->CallVoidMethod(mainActivityThis, nativeThreadMid);*/

    auto * context = new MyContext;
    //context->jniEnv是局部成员。并且线程之间不能直接传递env,即每个线程都附加了自己的env，不能传递给别的线程使用
    //context->jniEnv = env;
    //context->instance = mainActivityThis; //context->instance是局部成员
    //提升context->instance为全局成员
    context->instance = env->NewGlobalRef(mainActivityThis);

    /**
     * C语言创建线程的方式：
     * int pthread_create(
     *          pthread_t* __pthread_ptr,        //线程标记ID
     *          pthread_attr_t const* __attr,    //pthread配置的参数集，一般用不到
     *          void* (*__start_routine)(void*), //函数指针，相当于Java线程的run()
     *          void*                            //参数四会传递给参数三，作为函数指针的形参
     * );
     */
    pthread_t pid;
    pthread_create(&pid, nullptr, cpp_thread_run, context);

    //相当于Java的守护，即等待子线程工作做完
    //目的：为了下边的释放工作能在子线程工作做完后，再去释放内存。否则，程序会崩溃
    pthread_join(pid,nullptr);

    //todo 释放内存的工作
    env->DeleteGlobalRef(context->instance); //释放全局成员
    delete context; //释放对象
    context = nullptr;  //防止悬空指针
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sawyer_studyjni_MainActivity_closeThread(JNIEnv *env, jobject mainActivityThis) {
    //todo 做释放工作,释放全局引用
}


//=======================研究JavaVM、JNIEnv在不同线程的作用域=======================
extern "C"
JNIEXPORT void JNICALL
Java_com_sawyer_studyjni_MainActivity_nativeFun1(JNIEnv *env, jobject thiz) {
    JavaVM * javaVm = nullptr;
    env->GetJavaVM(&javaVm);
    LOGD("nativeFun1: JNIEnv地址 = %p, jvm地址 = %p, jobject地址 = %p, JNI_OnLoad的jvm地址 = %p",
         env, javaVm, thiz, ::jvm)
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sawyer_studyjni_MainActivity_nativeFun2(JNIEnv *env, jobject thiz) {
    JavaVM * javaVm = nullptr;
    env->GetJavaVM(&javaVm);
    LOGD("nativeFun2: JNIEnv地址 = %p, jvm地址 = %p, jobject地址 = %p, JNI_OnLoad的jvm地址 = %p",
         env, javaVm, thiz, ::jvm)
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sawyer_studyjni_MainActivity_staticFun3(JNIEnv *env, jclass clazz) {
    JavaVM * javaVm = nullptr;
    env->GetJavaVM(&javaVm);
    LOGD("staticFun3: JNIEnv地址 = %p, jvm地址 = %p, jclass地址 = %p, JNI_OnLoad的jvm地址 = %p",
         env, javaVm, clazz, ::jvm)
}

void * run(void *){
    JNIEnv * env = nullptr;
    ::jvm->AttachCurrentThread(&env, nullptr);
    LOGD("C++子线程：JNIEnv地址 = %p, jvm地址 = %p", env, ::jvm)
    ::jvm->DetachCurrentThread();
    return nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sawyer_studyjni_MainActivity_staticFun4(JNIEnv *env, jclass clazz) {
    JavaVM * javaVm = nullptr;
    env->GetJavaVM(&javaVm);
    LOGD("staticFun4: JNIEnv地址 = %p, jvm地址 = %p, jclass地址 = %p, JNI_OnLoad的jvm地址 = %p",
         env, javaVm, clazz, ::jvm)

    pthread_t pid;
    pthread_create(&pid, nullptr, run, nullptr);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sawyer_studyjni_SecondActivity_nativeFun5(JNIEnv *env, jobject thiz) {
    JavaVM * javaVm = nullptr;
    env->GetJavaVM(&javaVm);
    LOGD("nativeFun5: JNIEnv地址 = %p, jvm地址 = %p, jobject地址 = %p, JNI_OnLoad的jvm地址 = %p",
         env, javaVm, thiz, ::jvm)
}

/**
 * 研究JavaVM、JNIEnv在不同线程的作用域 -----日志结果
 * nativeFun1: JNIEnv地址 = 0xb400007c6f8df500, jvm地址 = 0xb400007c6f8ad380, jobject地址 = 0x7ff5ea64c8, JNI_OnLoad的jvm地址 = 0xb400007c6f8ad380
 * nativeFun2: JNIEnv地址 = 0xb400007c6f8df500, jvm地址 = 0xb400007c6f8ad380, jobject地址 = 0x7ff5ea64c8, JNI_OnLoad的jvm地址 = 0xb400007c6f8ad380
 * staticFun3: JNIEnv地址 = 0xb400007c6f8df500, jvm地址 = 0xb400007c6f8ad380, jclass地址 = 0x7c6e4a1208, JNI_OnLoad的jvm地址 = 0xb400007c6f8ad380
 * staticFun4: JNIEnv地址 = 0xb400007c6f921940, jvm地址 = 0xb400007c6f8ad380, jclass地址 = 0x7c6e4a1228, JNI_OnLoad的jvm地址 = 0xb400007c6f8ad380
 * C++子线程：JNIEnv地址 = 0xb400007c6f921940, jvm地址 = 0xb400007c6f8ad380
 * nativeFun5: JNIEnv地址 = 0xb400007c6f8df500, jvm地址 = 0xb400007c6f8ad380, jobject地址 = 0x7ff5ea6018, JNI_OnLoad的jvm地址 = 0xb400007c6f8ad380
 *
 * 结论：
 *  1.JavaVM是全局的，地址只有一个，它是绑定APP进程的
 *  2.JNIEnv是绑定Android主线程和子线程的，即Android的不同线程有自己的JNIEnv
 *  3.jobject是绑定当前实例的，即谁调用JNI函数，谁的实例会传给jobject
 *
 */