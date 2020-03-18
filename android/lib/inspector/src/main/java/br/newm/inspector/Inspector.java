package br.newm.inspector;

import android.annotation.SuppressLint;
import android.content.Context;

@SuppressWarnings("unused")
public class Inspector {
    static {
        System.loadLibrary("inspector");
    }

    @SuppressLint("StaticFieldLeak")
    private static Context context;

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

    // alias to avoid argument obfuscation of public method
    public static void setCipherKey(String database, String password, int version) {
        setCipherKeyJNI(database, password, version);
    }

    private static native void setCipherKeyJNI(String database, String password, int version);

    private static native void initialize(int port);
}
