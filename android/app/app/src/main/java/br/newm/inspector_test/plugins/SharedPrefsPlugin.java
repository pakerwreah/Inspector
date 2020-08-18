package br.newm.inspector_test.plugins;

import android.content.Context;
import android.content.SharedPreferences;

import org.json.JSONObject;

import androidx.preference.PreferenceManager;
import br.newm.inspector.PluginAction;

public class SharedPrefsPlugin implements PluginAction {

    private final SharedPreferences prefs;

    public SharedPrefsPlugin(Context context) {
        this.prefs = PreferenceManager.getDefaultSharedPreferences(context);
    }

    @Override
    public String action() {
        return new JSONObject(prefs.getAll()).toString();
    }
}
