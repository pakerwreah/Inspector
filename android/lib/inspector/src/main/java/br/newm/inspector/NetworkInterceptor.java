package br.newm.inspector;

import org.jetbrains.annotations.NotNull;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

import okhttp3.Headers;
import okhttp3.Interceptor;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;
import okhttp3.ResponseBody;
import okio.Buffer;

public class NetworkInterceptor implements Interceptor {
    @NotNull
    @Override
    public Response intercept(@NotNull Chain chain) throws IOException {
        String uid = UUID.randomUUID().toString();

        return processResponse(uid, chain, processRequest(uid, chain));
    }

    private Request processRequest(String uid, Chain chain) throws IOException {
        Request request = chain.request();
        RequestBody body = request.body();

        Map<String, String> headers = headerMap(request.headers());
        headers.put("_URL", request.url().toString());
        headers.put("_Method", request.method());

        Buffer buffer = new Buffer();
        if (body != null) {
            body.writeTo(buffer);
        }

        sendRequest(uid, buildHeaders(headers), buffer.readByteArray());

        return request;
    }

    private Response processResponse(String uid, Chain chain, Request request) throws IOException {
        try {
            Response response = chain.proceed(request);
            ResponseBody body = response.peekBody(Long.MAX_VALUE);

            byte bytes[] = {};

            if (body != null) {
                bytes = body.bytes();
            }

            boolean compressed = "gzip".equalsIgnoreCase(response.header("Content-Encoding"));

            Map<String, String> headers = headerMap(response.headers());
            headers.put("Status", String.valueOf(response.code()));

            sendResponse(uid, buildHeaders(headers), bytes, compressed);

            return response;
        } catch (Exception e) {
            Map<String, String> headers = new HashMap<>();
            headers.put("Status", "Error");
            sendResponse(uid, buildHeaders(headers), e.getLocalizedMessage().getBytes(), false);
            throw e;
        }
    }

    private Map<String, String> headerMap(Headers headers) {
        Map<String, String> map = new HashMap<>();
        for (String name : headers.names()) {
            map.put(name, headers.get(name));
        }
        return map;
    }

    private String buildHeaders(Map<String, String> headers) {
        ArrayList<String> keys = new ArrayList<>(headers.keySet());
        Collections.sort(keys);

        String headerstr = "";
        for (String _key : keys) {
            String key = _key;
            if (key.startsWith("_")) {
                key = _key.substring(1);
            }
            headerstr += key + ": " + headers.get(_key) + "\n";
        }
        return headerstr;
    }

    private native void sendRequest(String uid, String headers, byte body[]);

    private native void sendResponse(String uid, String headers, byte body[], boolean compressed);
}
