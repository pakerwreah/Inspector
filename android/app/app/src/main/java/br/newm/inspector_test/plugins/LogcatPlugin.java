package br.newm.inspector_test.plugins;

import android.content.Context;

import org.apache.commons.io.IOUtils;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;

import br.newm.inspector.Inspector;
import br.newm.inspector.PluginAction;

public class LogcatPlugin implements PluginAction {
    private String frontend = null;

    public LogcatPlugin(Context context) {
        try {
            this.frontend = IOUtils.toString(context.getAssets().open("logcat.html"), StandardCharsets.UTF_8);
            new Thread(new Runnable() {
                @Override
                public void run() {
                    logcat();
                }
            }).start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void logcat() {
        try {
            Process logcat = Runtime.getRuntime().exec(new String[]{
                    "logcat", "-v", "time", "-T", "0"
            });
            BufferedReader buffer = new BufferedReader(new InputStreamReader(logcat.getInputStream()));
            String line;
            while ((line = buffer.readLine()) != null) {
                Inspector.sendMessage("logcat", line);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public String action() {
        return frontend;
    }
}
