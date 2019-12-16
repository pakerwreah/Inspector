package br.newm.inspector;

import android.content.Context;

@SuppressWarnings("unused")
public class Inspector {
    private static Context context;

    static {
        System.loadLibrary("inspector");
    }

    public static void initializeWith(Context context, int port) {
        Inspector.context = context;

        initialize(port);
    }

    private static String[] databaseList() {
        String[] paths = context.databaseList();
        for (int i = 0; i < paths.length; i++) {
            paths[i] = context.getDatabasePath(paths[i]).getAbsolutePath();
        }
        return paths;
    }

    public static native void setCipherKey(String database, String password, int version);

    private static native void initialize(int port);
}
