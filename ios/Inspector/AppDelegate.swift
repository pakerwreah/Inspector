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

        IOSInspector.addPlugin("prefs", name: "User Defaults", plugin: UserDefaultsPlugin())
        IOSInspector.addLivePlugin("realtime", name: "Realtime", plugin: RealtimePlugin())
        IOSInspector.addLivePlugin("explorer", name: "Explorer", plugin: ExplorerPlugin())

        mockNetwork()
        mockMessages()
    }

    func mockNetwork() {
        DispatchQueue.global().async {
            let urls = [
                "https://google.com.br",
                "https://viacep.com.br/ws/01001000/json",
                "https://viacep.com.br/ws/15020035/json",
                "https://viacep.com.br/ws/1020035/json"
            ]
            var i = 0
            while(true) {
                let url = urls[i % urls.count]
                sleep(UInt32.random(in: 0...3))
                Http.request(url: url)
                i += 1
            }
        }
    }

    func mockMessages() {
        DispatchQueue.global().async {
            while(true) {
                sleep(UInt32.random(in: 0...3))
                let uid = UUID().uuidString
                IOSInspector.sendMessage(to: "realtime", message: "Message \(uid)")
            }
        }
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
