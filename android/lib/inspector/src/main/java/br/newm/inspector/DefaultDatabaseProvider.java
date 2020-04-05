package br.newm.inspector;

import android.content.Context;

import org.jetbrains.annotations.NotNull;

public class DefaultDatabaseProvider implements DatabaseProvider {
    private Context context;

    DefaultDatabaseProvider(@NotNull Context context) {
        this.context = context;
    }

    @Override
    public String[] databasePathList() {
        String[] paths = context.databaseList();
        for (int i = 0; i < paths.length; i++) {
            paths[i] = context.getDatabasePath(paths[i]).getAbsolutePath();
        }
        return paths;
    }
}
