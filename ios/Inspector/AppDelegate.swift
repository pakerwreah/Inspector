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

        DispatchQueue.global().async {
            while(true) {
                Http.request(url: "https://viacep.com.br/ws/15020340/json")
                Http.request(url: "https://viacep.com.br/ws/15020035/json")
                sleep(1)
                Http.request(url: "https://viacep.com.br/ws/1020035/json")
                sleep(5)
            }
        }
    }

    func databaseList() -> [String] {
        let documentsPathURL = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first!

        return ["banco_1.db", "banco_2.db", "banco_3.db"].map {
            documentsPathURL.appendingPathComponent($0).absoluteString
        }
    }
}
