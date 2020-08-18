package br.newm.inspector;

import java.util.Map;

public interface PluginAPIActionBinary {
    byte[] action(Map<String, String> params);
}
