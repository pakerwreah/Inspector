//
//  ExplorerPlugin.swift
//  Inspector
//
//  Created by Paker on 18/08/20.
//

import Foundation

class ExplorerPlugin: PluginActionProtocol {
    private lazy var frontend: String = {
        return Bundle.main
            .path(forResource: "explorer", ofType: "html")
            .flatMap { try? String(contentsOfFile: $0) } ?? ""
    }()

    private enum FileType: String, Encodable {
        case file, folder
    }

    private struct File: Encodable {
        let type: FileType
        let name: String
        let size: Int?
    }

    init(root: String? = nil) {
        guard let rootDirectory = (
            root.map {
                URL(fileURLWithPath: $0, isDirectory: true)
            } ?? FileManager.default
                .urls(for: .documentDirectory, in: .userDomainMask)
                .first
        ) else { return }

        let encoder = JSONEncoder()

        IOSInspector.addPluginAPI(forMethod: "GET", path: "filesystem/list") { params -> String in
            var output: String?
            do {
                let directory = rootDirectory.appendingPathComponent(params["path"] ?? "", isDirectory: true)

                let urls = try FileManager.default.contentsOfDirectory(at: directory,
                                                                       includingPropertiesForKeys: [.nameKey, .fileSizeKey, .isRegularFileKey],
                                                                       options: .skipsHiddenFiles)

                let files: [File] = try urls.compactMap { (file: URL) throws -> File? in
                    let attr = try file.resourceValues(forKeys: [.nameKey, .fileSizeKey, .isRegularFileKey])
                    guard let isFile = attr.isRegularFile, let name = attr.name else { return nil }
                    return File(type: isFile ? .file : .folder, name: name, size: attr.fileSize)
                }

                output = String(data: try encoder.encode(files), encoding: .utf8)
            } catch {
                print(error.localizedDescription)
            }
            return output ?? "[]"
        }

        IOSInspector.addPluginAPI(forMethod: "GET", path: "filesystem/open") { params -> Data? in
            var data: Data? = nil
            do {
                if let path = params["path"] {
                    data = try Data(contentsOf: rootDirectory.appendingPathComponent(path, isDirectory: false))
                }
            } catch {
                print(error.localizedDescription)
            }
            return data
        }
    }

    func action() -> String {
        return frontend
    }
}
