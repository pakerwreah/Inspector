package br.newm.inspector;

import android.os.Bundle;

import java.io.File;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        File touch = getDatabasePath("touch");
        touch.mkdirs();
        touch.delete();
    }

}
