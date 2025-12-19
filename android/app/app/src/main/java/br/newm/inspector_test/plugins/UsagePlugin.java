package br.pakerwreah.inspector_test.plugins;

import android.content.Context;

import org.apache.commons.io.IOUtils;
import org.json.JSONObject;

import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.charset.StandardCharsets;
import java.util.Timer;
import java.util.TimerTask;

import br.pakerwreah.inspector.Inspector;
import br.pakerwreah.inspector.PluginAction;

public class UsagePlugin implements PluginAction {
    private String frontend = null;
    private RandomAccessFile reader = null;
    private Runtime runtime = null;
    private CpuUsage old_stats = null;

    public UsagePlugin(Context context) {
        final long interval = 1000;
        try {
            this.frontend = IOUtils.toString(context.getAssets().open("usage.html"), StandardCharsets.UTF_8);

            runtime = Runtime.getRuntime();
            reader = new RandomAccessFile("/proc/stat", "r");
            old_stats = getCpuUsage();

            new Timer().schedule(new TimerTask() {
                @Override
                public void run() {
                    try {
                        CpuUsage stats = getCpuUsage();
                        final double cpuUsage = 100 * (stats.usage - old_stats.usage) / ((stats.usage + stats.idle) - (old_stats.usage + old_stats.idle));
                        old_stats = stats;

                        final JSONObject cpuObject = new JSONObject() {{
                            put("usage", cpuUsage);
                        }};

                        final MemoryUsage memoryUsage = getMemoryUsage();

                        final JSONObject memoryObject = new JSONObject() {{
                            put("used", memoryUsage.used);
                            put("max", memoryUsage.max);
                        }};

                        Inspector.sendMessage("usage", new JSONObject() {{
                            put("cpu", cpuObject);
                            put("memory", memoryObject);
                        }}.toString());
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }, interval, interval);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static class CpuUsage {
        float usage, idle;
    }

    private static class MemoryUsage {
        float used, max;
    }

    private CpuUsage getCpuUsage() throws IOException {
        reader.seek(0);
        String load = reader.readLine();
        String[] toks = load.split(" +");  // Split on one or more spaces

        CpuUsage usage = new CpuUsage();
        usage.idle = Long.parseLong(toks[4]);
        usage.usage = Long.parseLong(toks[2])
                + Long.parseLong(toks[3])
                + Long.parseLong(toks[5])
                + Long.parseLong(toks[6])
                + Long.parseLong(toks[7])
                + Long.parseLong(toks[8]);

        return usage;
    }

    private MemoryUsage getMemoryUsage() {
        MemoryUsage memory = new MemoryUsage();
        try {
            memory.used = (runtime.totalMemory() - runtime.freeMemory()) / 1048576f;
            memory.max = runtime.maxMemory() / 1048576f;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return memory;
    }

    @Override
    public String action() {
        return frontend;
    }
}
