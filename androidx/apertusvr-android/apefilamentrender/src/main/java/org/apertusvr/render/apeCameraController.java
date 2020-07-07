package org.apertusvr.render;

import android.annotation.SuppressLint;
import android.content.Context;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.SurfaceView;
import android.view.View;

import com.google.android.filament.Camera;

import org.apertusvr.apeVector3;

public class apeCameraController {

    private float rotateSpeed = 0.001f;
    private float speed = 0.05f;
    private float mHorizontal;
    private float mVertical;

    private apeVector3 mPosition;

    boolean lockAngles = false;

    private final float M_PI = 3.14159265f;

    private MotionState mMotionState;

    @SuppressLint("ClickableViewAccessibility")
    public apeCameraController(apeVector3 position, float horizontal, float vertical, SurfaceView surfaceView) {
        surfaceView.setOnTouchListener(new onTouchListener());
        mPosition = position;
        mHorizontal = horizontal;
        mVertical = vertical;
        mMotionState = MotionState.STOPPED;
    }

    public void setCameraTransform(Camera camera, float dt) {
        lockAngles = true;

        apeVector3 direction = new apeVector3(
                (float) (Math.cos(mVertical) * Math.sin(mHorizontal)),
                (float) Math.sin(mVertical),
                (float) (Math.cos(mVertical) * Math.cos(mHorizontal))
        );

        apeVector3 right = new apeVector3(
                (float) Math.sin(mHorizontal - M_PI / 2f),
                0f,
                (float) Math.cos(mHorizontal - M_PI / 2f)
        );

        lockAngles = false;

        if(mMotionState == MotionState.MOVE_FORWARD) {
            mPosition = mPosition.add(direction.scale(speed));
        }
        else if (mMotionState == MotionState.MOVE_BACKWARD) {
            mPosition = mPosition.add(direction.scale(-speed));
        }

        apeVector3 up = right.crossProduct(direction);

        camera.lookAt(mPosition.x, mPosition.y, mPosition.z,
                mPosition.x + direction.x, mPosition.y + direction.y, mPosition.z + direction.z,
                up.x,up.y, up.z);
    }

    private float clamp(float x, float a, float b) {
        if(x > b) return b;
        else if (x < a) return a;

        return x;
    }


    class onTouchListener implements View.OnTouchListener {

        float lastX;
        float lastY;

        @SuppressLint("ClickableViewAccessibility")
        @Override
        public boolean onTouch(View v, MotionEvent event) {


            if (event.getAction() == MotionEvent.ACTION_DOWN) {
                lastX = event.getX(0);
                lastY = event.getY(0);
            }
            else if (event.getAction() == MotionEvent.ACTION_MOVE && !lockAngles) {
                mHorizontal += rotateSpeed * (event.getX(0) - lastX);
                mVertical += rotateSpeed * (event.getY(0) - lastY);
                mVertical = clamp(mVertical,-M_PI / 2f, M_PI / 2f);

                lastX = event.getX();
                lastY = event.getY();
            }

            if (event.getPointerCount() == 2) {
                if (event.getY(1) < v.getHeight() / 2f) mMotionState = MotionState.MOVE_FORWARD;
                else mMotionState = MotionState.MOVE_BACKWARD;
            }
            else {
                mMotionState = MotionState.STOPPED;
            }


            return true;
        }
    }

    private enum MotionState {
        MOVE_FORWARD,
        MOVE_BACKWARD,
        STOPPED
    }

}
