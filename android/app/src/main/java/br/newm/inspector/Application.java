package br.newm.inspector;

public class Application extends android.app.Application {
    @Override
    public void onCreate() {
        super.onCreate();

        Inspector.initializeWithDefaults(this);
    }

    @Override
    public String[] databaseList() {
        return new String[]{"banco_1.db", "banco_2.db", "banco_3.db"};
    }
}
