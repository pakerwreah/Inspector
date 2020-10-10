package br.newm.inspector_test;

import br.newm.inspector.Inspector;
import br.newm.inspector_test.plugins.ExplorerPlugin;
import br.newm.inspector_test.plugins.LogcatPlugin;
import br.newm.inspector_test.plugins.SharedPrefsPlugin;

public class Application extends android.app.Application {
    @Override
    public void onCreate() {
        super.onCreate();

        Inspector.initializeWith(this);

        Inspector.setCipherKey("database_cipher3.db", "123456", 3);
        Inspector.setCipherKey("database_cipher4.db", "1234567", 4);

        Inspector.addPlugin("prefs", "Shared Preferences", new SharedPrefsPlugin(this));
        Inspector.addLivePlugin("logcat", "Logcat", new LogcatPlugin(this));
        Inspector.addLivePlugin("explorer", "Explorer", new ExplorerPlugin(this));
    }

    @Override
    public String[] databaseList() {
        return new String[]{"database.db", "database_cipher3.db", "database_cipher4.db"};
    }
}
