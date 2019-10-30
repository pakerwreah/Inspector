//
//  AppDelegate.swift
//  Inspector
//
//  Created by Paker on 29/10/19.
//  Copyright © 2019 NewM. All rights reserved.
//

import UIKit

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate, IOSInspectorProtocol {

    func applicationDidFinishLaunching(_ application: UIApplication) {
        IOSInspector.initialize(withDelegate: self, port: 30000)
    }

    func databaseList() -> [String] {
        let documentsPathURL = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first!

        return ["banco_1.db", "banco_2.db", "banco_3.db"].map {
            documentsPathURL.appendingPathComponent($0).absoluteString
        };
    }
}
