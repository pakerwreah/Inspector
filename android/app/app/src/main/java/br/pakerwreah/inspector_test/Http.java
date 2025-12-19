package br.pakerwreah.inspector_test;

import java.io.IOException;

import br.pakerwreah.inspector.NetworkInterceptor;
import okhttp3.Call;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.ResponseBody;

public class Http {
    private static final OkHttpClient.Builder builder = new OkHttpClient.Builder()
            // addNetworkInterceptor won't intercept timeouts
            .addInterceptor(new NetworkInterceptor());

    @SuppressWarnings("UnusedReturnValue")
    public static String request(String url) {
        OkHttpClient http = builder.build();

        Request.Builder req = new Request.Builder();
        Call call = http.newCall(req.url(url).build());
        try {
            ResponseBody body = call.execute().body();
            if (body != null) {
                return body.string();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        return "";
    }
}
