package br.newm.inspector_test.plugins;

import android.text.TextUtils;

import java.io.BufferedReader;
import java.io.InputStreamReader;

import br.newm.inspector.PluginAction;
import br.newm.inspector_test.BuildConfig;

public class LogcatPlugin implements PluginAction {

    private final int lines;

    public LogcatPlugin(int lines) {
        this.lines = lines;
    }

    @Override
    public String action() {
        Process logcat;
        final StringBuilder log = new StringBuilder();

        log.append("<div style='font-size: 12px'>");

        try {
            logcat = Runtime.getRuntime().exec(new String[]{
                    "logcat",
                    "-v", "time",
                    "-t", String.valueOf(lines),
                    BuildConfig.APPLICATION_ID + ":I"
            });
            BufferedReader br = new BufferedReader(new InputStreamReader(logcat.getInputStream()));
            String line;
            br.readLine(); // skip header
            while ((line = br.readLine()) != null) {
                log.append(TextUtils.htmlEncode(line)).append("<br />");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        log.append("</div>");

        return log.toString();
    }
}
