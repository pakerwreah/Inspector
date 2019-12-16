package br.newm.inspector_test;

import br.newm.inspector.Inspector;

public class Application extends android.app.Application {
    @Override
    public void onCreate() {
        super.onCreate();

        Inspector.initializeWith(this, 30000);

        Inspector.setCipherKey("database.db", "123456", 3);
    }

    @Override
    public String[] databaseList() {
        return new String[]{"database.db", "database2.db", "database3.db"};
    }
}
