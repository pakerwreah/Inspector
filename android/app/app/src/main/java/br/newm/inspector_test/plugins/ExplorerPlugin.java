package br.newm.inspector_test.plugins;

import android.content.Context;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.IOUtils;
import org.json.JSONArray;
import org.json.JSONObject;

import java.io.File;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.Map;

import br.newm.inspector.Inspector;
import br.newm.inspector.PluginAPIAction;
import br.newm.inspector.PluginAction;

public class ExplorerPlugin implements PluginAction {
    private String frontend = null;

    public ExplorerPlugin(final Context context, final String root) {
        try {
            this.frontend = IOUtils.toString(context.getAssets().open("explorer.html"), StandardCharsets.UTF_8);

            Inspector.addPluginAPI("GET", "filesystem/list", new PluginAPIAction() {
                @Override
                public String action(Map<String, String> params) {

                    JSONArray items = new JSONArray();
                    try {
                        String dir_path = params.get("path");
                        if (dir_path != null) {
                            File[] dir = new File(context.getFilesDir(), root + dir_path).listFiles();
                            if (dir != null) {
                                for (File item : dir) {
                                    JSONObject obj = new JSONObject();
                                    obj.put("type", item.isFile() ? "file" : "folder");
                                    obj.put("name", item.getName());
                                    obj.put("size", item.isFile() ? item.length() : 0);
                                    items.put(obj);
                                }
                            }
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    return items.toString();
                }
            });

            Inspector.addPluginAPI("GET", "filesystem/open", new PluginAPIAction() {
                @Override
                public String action(Map<String, String> params) {
                    String out = "";
                    try {
                        String path = params.get("path");
                        File file = new File(context.getFilesDir(), root + path);
                        out = FileUtils.readFileToString(file, StandardCharsets.UTF_8);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    return out;
                }
            });
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public String action() {
        return frontend;
    }
}
