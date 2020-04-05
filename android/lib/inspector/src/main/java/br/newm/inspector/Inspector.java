package br.newm.inspector;

import android.content.Context;

@SuppressWarnings({"unused", "WeakerAccess"})
public class Inspector {
    static {
        System.loadLibrary("inspector");
    }

    private static DatabaseProvider databaseProvider;

    public static void initializeWith(Context context, int port) {
        initializeWith(new DefaultDatabaseProvider(context), port);
    }

    public static void initializeWith(DatabaseProvider databaseProvider, int port) {
        Inspector.databaseProvider = databaseProvider;

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

    // Native methods

    private static native void setCipherKeyJNI(String database, String password, int version);

    private static native void addPluginJNI(String key, String name, PluginAction action);

    private static native void initialize(int port);

    // Obs.: there are redundant public methods because the arguments in native methods get obfuscated (don't know why)
}
