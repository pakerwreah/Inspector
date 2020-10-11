package br.newm.inspector;

import android.content.Context;
import android.content.pm.PackageManager;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

@SuppressWarnings({"unused", "RedundantSuppression"})
public class Inspector {
    static {
        System.loadLibrary("inspector");
    }

    private static DatabaseProvider databaseProvider;
    private static String packageName, versionName;

    public static void initializeWith(Context context) {
        initializeWith(context, 30000);
    }

    public static void initializeWith(Context context, int port) {
        initializeWith(context, new DefaultDatabaseProvider(context), port);
    }

    public static void initializeWith(Context context, DatabaseProvider databaseProvider, int port) {
        Inspector.databaseProvider = databaseProvider;

        try {
            packageName = context.getPackageName();
            versionName = context.getPackageManager().getPackageInfo(packageName, 0).versionName;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }

        initialize(port);
    }

    private static String[] databasePathList() {
        return databaseProvider.databasePathList();
    }

    // Public methods

    public static void setCipherKey(String database, String password, int version) {
        setCipherKeyJNI(database, password, version);
    }

    public static void addPlugin(String key, String name, PluginAction action) {
        addPluginJNI(key, name, action);
    }

    public static void addLivePlugin(String key, String name, PluginAction action) {
        addLivePluginJNI(key, name, action);
    }

    public static void addPluginAPI(String method, String path, final PluginAPIAction action) {
        addPluginAPIJNI(method, path, new PluginAPIActionJNI() {
            @Override
            public byte[] action(String params) {
                return action.action(decodeJSON(params)).getBytes();
            }
        });
    }

    public static void addPluginAPI(String method, String path, final PluginAPIActionBinary action) {
        addPluginAPIJNI(method, path, new PluginAPIActionJNI() {
            @Override
            public byte[] action(String params) {
                return action.action(decodeJSON(params));
            }
        });
    }

    public static void sendMessage(String key, String message) {
        sendMessageJNI(key, message);
    }

    private static Map<String, String> decodeJSON(String json) {
        HashMap<String, String> map = new HashMap<>();
        try {
            JSONObject obj = new JSONObject(json);
            Iterator<String> keys = obj.keys();
            while (keys.hasNext()) {
                String key = keys.next();
                map.put(key, obj.getString(key));
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return map;
    }

    private static String getPackageName() {
        return packageName;
    }

    private static String getVersionName() {
        return versionName;
    }

    // Native methods

    private interface PluginAPIActionJNI {
        byte[] action(String params);
    }

    private static native void setCipherKeyJNI(String database, String password, int version);

    private static native void addPluginJNI(String key, String name, PluginAction action);

    private static native void addLivePluginJNI(String key, String name, PluginAction action);

    private static native void addPluginAPIJNI(String method, String path, PluginAPIActionJNI action);

    private static native void sendMessageJNI(String key, String message);

    private static native void initialize(int port);

    // Obs.: there are redundant public methods because the arguments in native methods get obfuscated (don't know why)
}
