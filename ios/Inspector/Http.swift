//
// Created by Paker on 16/5/17.
//

import IOSInspector

enum Http {

    static func request(url: String) async throws {

        guard let url = URL(string: url) else {
            throw URLError(.badURL)
        }

        let request = URLRequest(url: url, cachePolicy: .reloadIgnoringCacheData, timeoutInterval: 3)

        let uid = UUID().uuidString

        IOSInspector.sendRequest(withUID: uid, request: request)

        do {
            let (data, response) = try await URLSession.shared.data(for: request)
            if let response = response as? HTTPURLResponse {
                IOSInspector.sendResponse(withUID: uid, response: response, body: data)
            }
        } catch {
            if let data = error.localizedDescription.data(using: .utf8) {
                let response = HTTPURLResponse(url: url, statusCode: 500, httpVersion: nil, headerFields: nil)
                IOSInspector.sendResponse(withUID: uid, response: response, body: data)
            }
        }
    }
}

