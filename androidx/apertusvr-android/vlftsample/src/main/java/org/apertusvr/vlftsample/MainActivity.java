package org.apertusvr.vlftsample;

import org.apertusvr.ApertusJNI;
import org.apertusvr.apeSceneNetwork;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.Objects;
import java.util.concurrent.TimeUnit;


public class MainActivity extends AppCompatActivity {

    private static final String LOG_TAG = "javalog.MainActivity";
    public static final String EXTRA_USERNAME = "org.apertusvr.vlftsample.USERNAME";
    public static final String EXTRA_ROOMNAME = "org.apertusvr.vlftsample.ROOMNAME";
    private static final String LOG_IN_CACHE_FILE = "lastLogIn.txt";

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
            File file = new File(getCacheDir(), LOG_IN_CACHE_FILE);
            try {
                FileOutputStream fos = new FileOutputStream(file);
                OutputStreamWriter osw = new OutputStreamWriter(fos);
                BufferedWriter bw = new BufferedWriter(osw);
                bw.write(userName + "\n" + roomName);
                bw.close();
            }
            catch (IOException e) {
                e.printStackTrace();
                Log.d(LOG_TAG, "WOW");
            }

            startActivity(intent);
            finish();
        }
        else {
            Toast toast = Toast.makeText(this, "Room is not available", Toast.LENGTH_SHORT);
            toast.show();
        }
    }

    public void loadCacheBtnPressed(View btn) {
        File file = new File(getCacheDir(), LOG_IN_CACHE_FILE);
        String userName = "";
        String roomName = "";
        try {
            FileInputStream fis = new FileInputStream(file);
            InputStreamReader isr = new InputStreamReader(fis);
            BufferedReader br = new BufferedReader(isr);
            userName = br.readLine();
            roomName = br.readLine();
            br.close();
        }
        catch (IOException e) {
            e.printStackTrace();
            Log.d(LOG_TAG, "WOW11");
        }

        EditText editUser = (EditText) findViewById(R.id.editUserName);
        editUser.setText(userName);
        EditText editRoom = (EditText) findViewById(R.id.editRoomName);
        editRoom.setText(roomName);
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
