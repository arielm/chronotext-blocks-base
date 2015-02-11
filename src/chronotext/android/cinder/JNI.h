/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2014, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

/*
 * TODO:
 *
 * 1) CHECK IF IT'S POSSIBLE TO MODERNIZE JNI USAGE:
 *    - E.G. USING "METHOD TABLES":
 *      - http://sbcgamesdev.blogspot.co.il/2012/12/using-jnionload-in-adroid-ndk.html
 *    - MORE JNI WISDOM HERE: http://developer.android.com/training/articles/perf-jni.html
 *
 * 2) TRY TO USE std::thread_local INSTEAD OF jni::getEnv()
 *
 * 3) STUDY FURTHER "JNI Local Reference Changes in ICS":
 *    - http://android-developers.blogspot.ie/2011/11/jni-local-reference-changes-in-ics.html
 *
 * 4) STUDY "Debugging Android JNI with CheckJNI":
 *    - http://android-developers.blogspot.co.il/2011/07/debugging-android-jni-with-checkjni.html
 */

#pragma once

#include <jni.h>

#include <string>

extern "C"
{
    jint JNI_OnLoad(JavaVM *vm, void *reserved);
    
    // ---

    void Java_org_chronotext_cinder_CinderBridge_init(JNIEnv *env, jobject obj, jobject bridge, jobject context, jobject display, jint displayWidth, jint displayHeight, jfloat displayDensity);
    void Java_org_chronotext_cinder_CinderBridge_uninit(JNIEnv *env, jobject obj);
    void Java_org_chronotext_cinder_CinderRenderer_setup(JNIEnv *env, jobject obj, jint width, jint height);
    void Java_org_chronotext_cinder_CinderRenderer_shutdown(JNIEnv *env, jobject obj);

    void Java_org_chronotext_cinder_CinderRenderer_resize(JNIEnv *env, jobject obj, jint width, jint height);
    void Java_org_chronotext_cinder_CinderRenderer_draw(JNIEnv *env, jobject obj);
    
    void Java_org_chronotext_cinder_CinderRenderer_addTouch(JNIEnv *env, jobject obj, jint index, jfloat x, jfloat y);
    void Java_org_chronotext_cinder_CinderRenderer_updateTouch(JNIEnv *env, jobject obj, jint index, jfloat x, jfloat y);
    void Java_org_chronotext_cinder_CinderRenderer_removeTouch(JNIEnv *env, jobject obj, jint index, jfloat x, jfloat y);
    
    void Java_org_chronotext_cinder_CinderRenderer_dispatchEvent(JNIEnv *env, jobject obj, jint eventId);
    void Java_org_chronotext_cinder_CinderBridge_sendMessageToSketch(JNIEnv *env, jobject obj, jint what, jstring body);
}

// ---

namespace chr
{
    namespace jni
    {
        extern JavaVM *vm;
        extern jobject bridge;
        
        // ---
        
        JNIEnv* getEnv();
        
        std::string toString(jstring s);
        jstring toJString(const std::string &s);
        
        void callVoidMethodOnBridge(const char *name, const char *sig, ...);
        jboolean callBooleanMethodOnBridge(const char *name, const char *sig, ...);
        jchar callCharMethodOnBridge(const char *name, const char *sig, ...);
        jint callIntMethodOnBridge(const char *name, const char *sig, ...);
        jlong callLongMethodOnBridge(const char *name, const char *sig, ...);
        jfloat callFloatMethodOnBridge(const char *name, const char *sig, ...);
        jdouble callDoubleMethodOnBridge(const char *name, const char *sig, ...);
        jobject callObjectMethodOnBridge(const char *name, const char *sig, ...);
    }
}
