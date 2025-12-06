//
//  AppDelegate.swift
//  Inspector
//
//  Created by Paker on 29/10/19.
//

import UIKit
import IOSInspector

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate, IOSInspectorProtocol {

    func applicationDidFinishLaunching(_ application: UIApplication) {

        IOSInspector.initialize(withDelegate: self)

        IOSInspector.setCipherKey("database_cipher3.db", password: "123456", version: 3)
        IOSInspector.setCipherKey("database_cipher4.db", password: "1234567", version: 4)

        IOSInspector.addPlugin("prefs", name: "User Defaults", plugin: UserDefaultsPlugin())
        IOSInspector.addLivePlugin("realtime", name: "Realtime", plugin: RealtimePlugin())
        IOSInspector.addLivePlugin("explorer", name: "Explorer", plugin: ExplorerPlugin())

        mockNetwork()
        mockMessages()
        mockDatabases()
    }
    
    func mockNetwork() {
        Task {
            let urls = [
                "https://google.com.br",
                "https://viacep.com.br/ws/01001000/json",
                "https://viacep.com.br/ws/15020035/json",
                "https://viacep.com.br/ws/1020035/json"
            ]
            var i = 0
            while(true) {
                let url = urls[i % urls.count]
                try await Task.sleep(for: .seconds(.random(in: 0...5)))
                try await Http.request(url: url)
                i += 1
            }
        }
    }

    func mockMessages() {
        Task {
            while(true) {
                try await Task.sleep(for: .seconds(.random(in: 0...3)))
                IOSInspector.sendMessage(to: "realtime", message: "Message \(UUID().uuidString)")
            }
        }
    }
    
    func mockDatabases() {
        func path(_ database: String) -> String {
            documentDirectory.appendingPathComponent(database).absoluteString
        }
        IOSInspector.createDatabase(path("database.db"))
        IOSInspector.createDatabase(path("database_cipher3.db"), password: "123456", version: 3)
        IOSInspector.createDatabase(path("database_cipher4.db"), password: "1234567", version: 4)
    }

    lazy var documentDirectory: URL = {
        let url = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first!
        print(url.absoluteString)
        return url
    }()

    func databaseList() -> [String] {
        return ["database.db", "database_cipher3.db", "database_cipher4.db"].map {
            documentDirectory.appendingPathComponent($0).absoluteString
        }
    }
}
