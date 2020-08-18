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
        if let data = try? JSONSerialization.data(withJSONObject: dict),
            let json = String(data: data, encoding: .utf8) {
            return json
        }
        return "No data"
    }
}
