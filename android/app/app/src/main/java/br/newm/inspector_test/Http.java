package br.newm.inspector_test;

import java.io.IOException;

import br.newm.inspector.NetworkInterceptor;
import okhttp3.Call;
import okhttp3.OkHttpClient;
import okhttp3.Request;

public class Http {
    private static final OkHttpClient.Builder builder = new OkHttpClient.Builder()
            .addNetworkInterceptor(new NetworkInterceptor());

    public static String request(String url) {
        OkHttpClient http = builder.build();

        Request.Builder req = new Request.Builder();
        Call call = http.newCall(req.url(url).build());
        try {
            return call.execute().body().string();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return "";
    }
}
