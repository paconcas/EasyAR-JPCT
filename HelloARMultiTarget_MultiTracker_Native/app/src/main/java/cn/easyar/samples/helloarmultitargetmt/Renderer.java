/**
* Copyright (c) 2015-2016 VisionStar Information Technology (Shanghai) Co., Ltd. All Rights Reserved.
* EasyAR is the registered trademark or trademark of VisionStar Information Technology (Shanghai) Co., Ltd in China
* and other countries for the augmented reality technology developed by VisionStar Information Technology (Shanghai) Co., Ltd.
*/

package cn.easyar.samples.helloarmultitargetmt;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import com.threed.jpct.Camera;
import com.threed.jpct.FrameBuffer;
import com.threed.jpct.Interact2D;
import com.threed.jpct.Light;
import com.threed.jpct.Loader;
import com.threed.jpct.Matrix;
import com.threed.jpct.Object3D;
import com.threed.jpct.SimpleVector;
import com.threed.jpct.Texture;
import com.threed.jpct.TextureManager;
import com.threed.jpct.World;
import com.threed.jpct.util.MemoryHelper;


import android.content.res.Resources;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.widget.Toast;

import java.io.InputStream;
import java.io.UnsupportedEncodingException;

public class Renderer implements GLSurfaceView.Renderer {

    private GLSurfaceView.Renderer mRenderer;
    private Texture texture;
    private TextureManager textureManager;
    private Object3D cube;
    private static MainActivity mActivity;
    static FrameBuffer  fb;
    private float[] modelViewMat;
    private static float fov;
    private static float fovy;
    private static int targetID = -1;
    private static World world;
    private static Light sun;
    private static Camera cam;
    private static Object3D torreon;
    private static String TAG = "Debug";


    public Renderer(MainActivity activity) {

        mActivity = activity;

        //------------------------------//
        //------------------------------//
        //			LOAD 3D MODEL   	//
        //------------------------------//
        //------------------------------//

        world = new World();
        world.setAmbientLight(150, 150, 150);

        sun = new Light(world);
        sun.setIntensity(250, 250, 250);


        //Load textures for the panel
        loadTorreonTextures();

        try {

            torreon = loadModelOBJ(mActivity.getResources().openRawResource(R.raw.torreon_obj),
                    mActivity.getResources().openRawResource(R.raw.torreon_mat), 0.5f);

            torreon.setCollisionMode(Object3D.COLLISION_CHECK_OTHERS|Object3D.COLLISION_CHECK_SELF);


        } catch (UnsupportedEncodingException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (Resources.NotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        world.addObjects(torreon);

        cam = world.getCamera();

        SimpleVector sv_t = new SimpleVector();
        sv_t.set(torreon.getTransformedCenter());
        sv_t.y += 10;
        sv_t.z += 10;
        sun.setPosition(sv_t);
        MemoryHelper.compact();
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        MainActivity.nativeInitGL();
    }

    public void onSurfaceChanged(GL10 gl, int w, int h) {
        MainActivity.nativeResizeGL(w, h);

        if (fb != null) {
            fb.dispose();
        }
        fb = new FrameBuffer(w, h);
    }

    public void onDrawFrame(GL10 gl) {

        modelViewMat = MainActivity.nativeRender();

        if (targetID == 0) {
            Log.d(TAG,"Target 0");
        }
        else if(targetID == 1)
        {
            Log.d(TAG,"Target 1");
        }

        // SET-UP THE WORLD WITH THE MAP
        updateCamera(cam);

        world.renderScene(fb);
        world.draw(fb);
        fb.display();

        }

    public void updateCamera(Camera _cam) {


        if (modelViewMat != null) {
            float[] m = modelViewMat;

            final SimpleVector camUp;


            camUp = new SimpleVector(m[4], m[5], -m[6]);
            final SimpleVector camDirection = new SimpleVector(-m[8], -m[9], m[10]);
            final SimpleVector camPosition = new SimpleVector(m[12]  , m[13] , -m[14]);

            _cam.setOrientation(camDirection, camUp);
            _cam.setPosition(camPosition);

            _cam.setFOV(fov);
            _cam.setYFOV(fovy);

            _cam.adjustFovToNearPlane();

        }
    }

    public static void setFov(float _fov) {
        fov = _fov;
    }

    public static void setFovy(float _fovy) {
        fovy = _fovy;
    }

    public static void getTarget(int _targetID)
    {
        targetID = _targetID;
    }

    private Object3D loadModelOBJ(InputStream inputStream, InputStream inputStream2, float scale) throws UnsupportedEncodingException {

        Object3D[] model = Loader.loadOBJ(inputStream, inputStream2, scale);
        Object3D o3d = new Object3D(0);
        Object3D temp = null;
        for (int i = 0; i < model.length; i++) {
            temp = model[i];
            temp.setCenter(SimpleVector.ORIGIN);
            temp.rotateX((float)(-.5*Math.PI));
            temp.rotateMesh();
            temp.setRotationMatrix(new Matrix());
            o3d = Object3D.mergeObjects(o3d, temp);
            o3d.build();
        }
        return o3d;
    }

    private void loadTorreonTextures()
    {

        TextureManager txtMgr = TextureManager.getInstance();
        //TEXTURAS TORREON

        if (!txtMgr.containsTexture("torre_diffuse6.jpg")) {
            Texture torre_diffuse6 = new Texture(mActivity.getResources().openRawResource(R.raw.torre_diffuse6));
            txtMgr.addTexture("torre_diffuse6.jpg", torre_diffuse6);
        }

        if (!txtMgr.containsTexture("torre_normals.jpg")) {
            Texture torre_normals = new Texture(mActivity.getResources().openRawResource(R.raw.torre_normals));
            txtMgr.addTexture("torre_normals.jpg", torre_normals);
        }

    }

    public static void touchObject(int x, int y)
    {
        SimpleVector dir = Interact2D.reproject2D3DWS(cam, fb, x, y).normalize();
        Object[] res= world.calcMinDistanceAndObject3D(cam.getPosition(), dir,
                100000000 /* a distance: picking will be ignoder if larger */);
        // cast the result
        Float    pickedDistance = (Float)    res[0];
        Object3D pickedObject   = (Object3D) res[1];

        // check if something has been picked immediately after
        if (pickedDistance != Object3D.COLLISION_NONE) {

            if (pickedObject.getID() == torreon.getID()) {

                Toast.makeText(mActivity.getApplicationContext(),"Object touched!",Toast.LENGTH_LONG).show();

            }


        }
    }
}
