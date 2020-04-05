package br.newm.inspector_test;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;

import java.io.File;
import java.util.Timer;
import java.util.TimerTask;

import androidx.appcompat.app.AppCompatActivity;
import androidx.preference.PreferenceManager;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        File touch = getDatabasePath("touch");
        touch.mkdirs();
        touch.delete();

        final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);

        SharedPreferences.Editor editor = prefs.edit();
        editor.putString("test_1", "abcde");
        editor.putInt("test_2", 12345);
        editor.putBoolean("test_3", true);
        editor.putFloat("test_4", 0.12345f);
        editor.apply();

        new Thread(new Runnable() {
            @Override
            public void run() {
                String urls[] = {
                        "https://google.com.br",
                        "https://viacep.com.br/ws/01001000/json",
                        "https://viacep.com.br/ws/15020035/json",
                        "https://viacep.com.br/ws/1020035/json"
                };
                for (int i = 0; ; i++) {
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    String resp = Http.request(urls[i % urls.length]);

                    Log.d("Inspector", resp);
                }
            }
        }).start();
    }

}
