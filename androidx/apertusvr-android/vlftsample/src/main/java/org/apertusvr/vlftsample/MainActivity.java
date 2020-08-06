package org.apertusvr.vlftsample;

import org.apertusvr.ApertusJNI;
import org.apertusvr.apeSceneNetwork;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Objects;
import java.util.concurrent.TimeUnit;


public class MainActivity extends AppCompatActivity {

    private static final String LOG_TAG = "javalog.MainActivity";
    public static final String EXTRA_USERNAME = "org.apertusvr.vlftsample.USERNAME";
    public static final String EXTRA_ROOMNAME = "org.apertusvr.vlftsample.ROOMNAME";
    private static final String LAST_SETTINGS_FILE = "lastSettings.txt";

    ApertusJNI apeJNI = new ApertusJNI();
    public static boolean apeStarted = false;


    @SuppressLint("ClickableViewAccessibility")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Objects.requireNonNull(getSupportActionBar()).hide();

        setContentView(R.layout.activity_main);

        if (!apeStarted) {
            Log.d(LOG_TAG, "START APEVR");
            apeJNI.startApertusVR("vlftGuest",getAssets());
            apeStarted = true;
        }
    }

    public void connectBtnPressed(View btn) {
        Intent intent = new Intent(this, GameActivity.class);

        EditText editUser = (EditText) findViewById(R.id.editUserName);
        String userName = editUser.getText().toString();
        EditText editRoom = (EditText) findViewById(R.id.editRoomName);
        String roomName = editRoom.getText().toString();

        if(apeSceneNetwork.isRoomRunning(roomName)) {
            intent.putExtra(EXTRA_USERNAME, userName);
            intent.putExtra(EXTRA_ROOMNAME, roomName);

            // TODO: cache the userName and roomName
            //File file = new File(getCacheDir(), LAST_SETTINGS_FILE);


            startActivity(intent);
            finish();
        }
        else {
            Toast toast = Toast.makeText(this, "Room is not available", Toast.LENGTH_SHORT);
            toast.show();
        }
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }


}
