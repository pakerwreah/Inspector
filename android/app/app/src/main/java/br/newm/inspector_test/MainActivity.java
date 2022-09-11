package br.newm.inspector_test;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.widget.CompoundButton;
import android.widget.ToggleButton;

import java.io.File;
import java.util.ArrayList;

import androidx.appcompat.app.AppCompatActivity;
import androidx.preference.PreferenceManager;

public class MainActivity extends AppCompatActivity {

    boolean simulatingUsage = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        createDatabaseFolder();

        mockPrefs();

        mockNetwork();

        ToggleButton btn_simulate_usage = findViewById(R.id.btn_simulate_usage);

        btn_simulate_usage.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton cb, boolean checked) {
                simulatingUsage = checked;
                if (checked) {
                    simulateUsage();
                }
            }
        });
    }

    private void createDatabaseFolder() {
        File touch = getDatabasePath("touch");
        touch.mkdirs();
        touch.delete();
    }

    private void mockPrefs() {
        final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);

        SharedPreferences.Editor editor = prefs.edit();
        editor.putString("test_1", "abcde");
        editor.putInt("test_2", 12345);
        editor.putBoolean("test_3", true);
        editor.putFloat("test_4", 0.12345f);
        editor.apply();
    }

    private void mockNetwork() {
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
                        //noinspection BusyWait
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    String url = urls[i % urls.length];

                    Http.request(url);

                    Log.d("Inspector", url);
                }
            }
        }).start();
    }

    public void simulateUsage() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                ArrayList<String> arr = new ArrayList<>();
                for (int i = 0; simulatingUsage; i++) {
                    arr.add("Lorem ipsum dolor sit amet, consectetur adipiscing elit." + i);
                    System.out.println("Simulating usage");
                }
                System.out.println("String usage: " + arr.size());
            }
        }).start();
    }
}
