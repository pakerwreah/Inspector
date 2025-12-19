package br.pakerwreah.inspector;

import java.util.Map;

public interface PluginAPIAction {
    String action(Map<String, String> params);
}
