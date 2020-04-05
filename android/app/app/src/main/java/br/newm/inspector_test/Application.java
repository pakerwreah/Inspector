package br.newm.inspector_test;

import android.content.SharedPreferences;

import org.json.JSONObject;
import org.json.JSONStringer;

import androidx.preference.PreferenceManager;

import br.newm.inspector.Inspector;
import br.newm.inspector.PluginAction;

public class Application extends android.app.Application {
    @Override
    public void onCreate() {
        super.onCreate();

        Inspector.initializeWith(this, 30000);

        Inspector.setCipherKey("database_cipher3.db", "123456", 3);
        Inspector.setCipherKey("database_cipher4.db", "1234567", 4);

        final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);

        Inspector.addPlugin("prefs", "Shared Preferences", new PluginAction() {
            @Override
            public String action() {
                return new JSONObject(prefs.getAll()).toString();
            }
        });
    }

    @Override
    public String[] databaseList() {
        return new String[]{"database.db", "database_cipher3.db", "database_cipher4.db"};
    }
}
