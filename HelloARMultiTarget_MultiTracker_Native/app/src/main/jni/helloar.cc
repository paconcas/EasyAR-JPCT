/**
* Copyright (c) 2015-2016 VisionStar Information Technology (Shanghai) Co., Ltd. All Rights Reserved.
* EasyAR is the registered trademark or trademark of VisionStar Information Technology (Shanghai) Co., Ltd in China
* and other countries for the augmented reality technology developed by VisionStar Information Technology (Shanghai) Co., Ltd.
*/

#include "ar.hpp"
#include "utils.hpp"
#include <jni.h>
#include <GLES2/gl2.h>

#define JNIFUNCTION_NATIVE(sig) Java_cn_easyar_samples_helloarmultitargetmt_MainActivity_##sig

extern "C" {
    JNIEXPORT jboolean JNICALL JNIFUNCTION_NATIVE(nativeInit(JNIEnv* env, jobject object));
    JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeDestory(JNIEnv* env, jobject object));
    JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeInitGL(JNIEnv* env, jobject object));
    JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeResizeGL(JNIEnv* env, jobject object, jint w, jint h));
    JNIEXPORT jfloatArray JNICALL JNIFUNCTION_NATIVE(nativeRender(JNIEnv* env, jobject obj));
    JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeRotationChange(JNIEnv* env, jobject obj, jboolean portrait));
};

namespace EasyAR {
    namespace samples {

        class HelloAR : public AR
        {
        public:
            HelloAR();
            virtual void initGL();
            virtual void resizeGL(int width, int height);
            virtual int render(Matrix44F &cameraviewArray, float &fovyRadians , float &fovRadians);
        private:
            Vec2I view_size;
        };

        HelloAR::HelloAR()
        {
            view_size[0] = -1;
        }

        void HelloAR::initGL()
        {
            augmenter_ = Augmenter();
            augmenter_.attachCamera(camera_);
        }

        void HelloAR::resizeGL(int width, int height)
        {
            view_size = Vec2I(width, height);
        }

        int HelloAR::render(Matrix44F &cameraview, float &fovyRadians, float &fovRadians)
        {
            int targetid = -1;

            std::string str[] = {"target0", "target1"};

            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_DEPTH_TEST);


            Frame frame = augmenter_.newFrame();
            if(view_size[0] > 0){
                AR::resizeGL(view_size[0], view_size[1]);
                if(camera_ && camera_.isOpened())
                    view_size[0] = -1;
            }
            augmenter_.setViewPort(viewport_);
            augmenter_.drawVideoBackground();
            glViewport(viewport_[0], viewport_[1], viewport_[2], viewport_[3]);

            Vec2I size = camera_.cameraCalibration().size();
            Vec2F focalLength = camera_.cameraCalibration().focalLength();

            fovyRadians = size.data[1] / focalLength.data[1];
            fovRadians =  size.data[0] / focalLength.data[0];


            for (int i = 0; i < frame.targets().size(); ++i) {
                AugmentedTarget::Status status = frame.targets()[i].status();
                if (status == AugmentedTarget::kTargetStatusTracked) {

                    cameraview = getPoseGL(frame.targets()[i].pose());
                    ImageTarget target = frame.targets()[i].target().cast_dynamic<ImageTarget>();

                    for(int j = 0; j < 2; ++j) {
                        if (str[j].compare(target.name()) == 0) {
                            targetid = j;
                        }
                    }
                }
            }

            return targetid;
        }

    }
}
EasyAR::samples::HelloAR ar;

JNIEXPORT jboolean JNICALL JNIFUNCTION_NATIVE(nativeInit(JNIEnv*, jobject))
{
    bool status = ar.initCamera();
    ar.loadAllFromJsonFile("targets.json",0);
    status &= ar.start();
    return status;
}

JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeDestory(JNIEnv*, jobject))
{
    ar.clear();
}

JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeInitGL(JNIEnv*, jobject))
{
    ar.initGL();
}

JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeResizeGL(JNIEnv*, jobject, jint w, jint h))
{
    ar.resizeGL(w, h);
}

JNIEXPORT jfloatArray JNICALL JNIFUNCTION_NATIVE(nativeRender(JNIEnv* env, jobject))
{
    jfloatArray modelviewArray;


    float fovyRadians;
    float fovRadians;
    jclass activityClass = env->FindClass("cn/easyar/samples/helloarmultitargetmt/Renderer");
    jmethodID fovmethodID = env->GetStaticMethodID(activityClass, "setFov", "(F)V");
    jmethodID fovYmethodID = env->GetStaticMethodID(activityClass, "setFovy", "(F)V");
    jmethodID getTargetID = env->GetStaticMethodID(activityClass, "getTarget", "(I)V");


    modelviewArray = env->NewFloatArray(16);
    if (modelviewArray == NULL) {
        return NULL; /* out of memory error thrown */
    }
    EasyAR::Matrix44F modelview;

    int targetid;

    targetid = ar.render(modelview,fovyRadians,fovRadians);

    utils::rotatePoseMatrix(180.0f, 0, 1.0f, 0, &modelview.data[0]);
    utils::scalePoseMatrix(0.1f,0.1f,0.1f,&modelview.data[0]);

    EasyAR::Matrix44F modelview_inv = utils::Matrix44FInverse(modelview);
    EasyAR::Matrix44F modelview_inv_trans = utils::Matrix44FTranspose(modelview_inv);

    env->SetFloatArrayRegion(modelviewArray, 0, 16, modelview_inv_trans.data);


    env->CallStaticVoidMethod(activityClass,fovmethodID,fovRadians);
    env->CallStaticVoidMethod(activityClass,fovYmethodID,fovyRadians);
    env->CallStaticVoidMethod(activityClass,getTargetID,targetid);

    return modelviewArray;

}

JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeRotationChange(JNIEnv*, jobject, jboolean portrait))
{
    ar.setPortrait(portrait);
}
