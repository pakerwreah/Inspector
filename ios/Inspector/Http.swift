//
// Created by Paker on 16/5/17.
//

import UIKit
import Foundation
import SystemConfiguration

extension Data {
    var string: String? {
        return String(data: self, encoding: String.Encoding.utf8)
    }
}

class Http {

    class func request(url: String) {
        assert(!Thread.isMainThread, "Network on main thread!")

        let request = URLRequest(url: URL(string: url)!, cachePolicy: .reloadIgnoringCacheData)

        let uid = UUID().uuidString

        let session = URLSession(configuration: .default)

        let semaphore = DispatchSemaphore(value: 0)

        IOSInspector.sendRequest(withUID: uid, request: request)

        session.dataTask(with: request) { (data: Data?, response: URLResponse?, error: Error?) in
            if let error = error {
                print(error.localizedDescription)
            }
            else if let data = data, let response = response as? HTTPURLResponse {
                IOSInspector.sendResponse(withUID: uid, response: response, body: data)
            }
            semaphore.signal()
        }.resume()

        semaphore.wait()
    }
}

