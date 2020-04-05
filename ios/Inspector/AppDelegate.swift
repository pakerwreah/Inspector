//
//  AppDelegate.swift
//  Inspector
//
//  Created by Paker on 29/10/19.
//

import UIKit

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate, IOSInspectorProtocol {

    func applicationDidFinishLaunching(_ application: UIApplication) {
        IOSInspector.initialize(withDelegate: self, port: 30000)

        IOSInspector.setCipherKey("database_cipher3.db", password: "123456", version: 3)
        IOSInspector.setCipherKey("database_cipher4.db", password: "1234567", version: 4)

        IOSInspector.addPlugin("prefs", name: "User Defaults") {
            let dict = UserDefaults.standard.dictionaryRepresentation()
            if let data = try? JSONSerialization.data(withJSONObject: dict),
                let json = String(data: data, encoding: .utf8) {
                return json
            }
            return "No data"
        }

        mockNetwork()
    }

    func mockNetwork() {
        DispatchQueue.global().async {
            while(true) {
                Http.request(url: "https://viacep.com.br/ws/01001000/json")
                Http.request(url: "https://viacep.com.br/ws/15020035/json")
                sleep(1)
                Http.request(url: "https://viacep.com.br/ws/1020035/json")
                sleep(5)
            }
        }
    }

    func databaseList() -> [String] {
        let documentsPathURL = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first!

        print(documentsPathURL.absoluteString)

        return ["database.db", "database_cipher3.db", "database_cipher4.db"].map {
            documentsPathURL.appendingPathComponent($0).absoluteString
        }
    }
}
