package br.newm.inspector;

import okhttp3.*;
import okio.Buffer;
import org.jetbrains.annotations.NotNull;

import java.io.IOException;
import java.util.*;

@SuppressWarnings("unused")
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
            ResponseBody body = response.body();

            byte[] bytes = {};

            if (body != null) {
                bytes = response.peekBody(Long.MAX_VALUE).bytes();
            }

            boolean compressed = "gzip".equalsIgnoreCase(response.header("Content-Encoding"));

            Map<String, String> headers = headerMap(response.headers());
            headers.put("_Status", String.valueOf(response.code()));

            sendResponse(uid, buildHeaders(headers), bytes, compressed);

            return response;
        } catch (Exception e) {
            Map<String, String> headers = new HashMap<>();
            headers.put("_Status", "Error");
            String msg = e.getLocalizedMessage();
            sendResponse(uid, buildHeaders(headers), (msg != null ? msg : "").getBytes(), false);
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
        Collections.sort(keys, String.CASE_INSENSITIVE_ORDER);

        StringBuilder headerstr = new StringBuilder();
        for (String _key : keys) {
            String key = _key;
            if (key.startsWith("_")) {
                key = _key.substring(1);
            }
            headerstr.append(key).append(": ").append(headers.get(_key)).append("\n");
        }
        return headerstr.toString();
    }

    private native void sendRequest(String uid, String headers, byte[] body);

    private native void sendResponse(String uid, String headers, byte[] body, boolean compressed);
}
