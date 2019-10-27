package br.newm.inspector;

import android.content.Context;

@SuppressWarnings("unused")
public class Inspector {
    private static Context context;

    static {
        System.loadLibrary("inspector");
    }

    public static void initializeWithDefaults(Context context) {
        Inspector.context = context;

        initialize();
    }

    private static String[] databaseList() {
        String[] paths = context.databaseList();
        for (int i = 0; i < paths.length; i++) {
            paths[i] = context.getDatabasePath(paths[i]).getAbsolutePath();
        }
        return paths;
    }

    private static native void initialize();
}
