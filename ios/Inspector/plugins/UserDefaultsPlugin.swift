//
//  UserDefaultsPlugin.swift
//  Inspector
//
//  Created by Paker on 18/08/20.
//

import IOSInspector

class UserDefaultsPlugin: PluginActionProtocol {
    func action() -> String {
        if
            let dict = encodableDict(UserDefaults.standard.dictionaryRepresentation()),
            let data = try? JSONSerialization.data(withJSONObject: dict),
            let json = String(data: data, encoding: .utf8)
        {
            return json
        }
        return "No data"
    }
}

func encodableDict(_ dict: [String: Any]) -> [String: Any]? {
    return dict.mapValues { (value) -> Any? in
        if let value = value as? [String: Any] {
            return encodableDict(value)
        } else if JSONSerialization.isValidJSONObject(value) {
            return value
        } else {
            return "\(value)"
        }
    }
}
