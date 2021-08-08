//
//  RealtimePlugin.swift
//  Inspector
//
//  Created by Paker on 26/09/20.
//

#if canImport(IOSInspector)
import IOSInspector
#endif

class RealtimePlugin: PluginActionProtocol {
    private lazy var frontend: String = {
        return Bundle.main
            .path(forResource: "realtime", ofType: "html")
            .flatMap { try? String(contentsOfFile: $0) } ?? ""
    }()
    
    func action() -> String {
        return frontend
    }
}
