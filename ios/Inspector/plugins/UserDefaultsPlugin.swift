//
//  UserDefaultsPlugin.swift
//  Inspector
//
//  Created by Paker on 18/08/20.
//

import Foundation

class UserDefaultsPlugin: PluginActionProtocol {
    func action() -> String {
        let dict = UserDefaults.standard.dictionaryRepresentation()
        if let data = try? JSONSerialization.data(withJSONObject: encodableDict(dict)),
            let json = String(data: data, encoding: .utf8) {
            return json
        }
        return "No data"
    }
}

func encodableDict(_ dict: [String: Any]) -> [String: Any] {
    return dict.mapValues { (value) -> Any in
        if value is Date {
            // serialize Date manually
            return "\(value)"
        } else if let value = value as? [String: Any] {
            return encodableDict(value)
        }
        return value
    }
}
