// swift-tools-version:5.4

import PackageDescription

let package = Package(
    name: "IOSInspector",
    products: [
        .library(
            name: "IOSInspector",
            targets: ["IOSInspector"]
        )
    ],
    dependencies: [
        .package(url: "https://github.com/sqlcipher/SQLCipher.swift.git", from: "4.11.0")
    ],
    targets: [
        .target(
            name: "IOSInspector",
            dependencies: ["Inspector"],
            path: ".",
            sources: ["ios/Inspector/lib"],
            publicHeadersPath: "ios/Inspector/lib",
            cxxSettings: [
                .headerSearchPath("./cpp/src"),
                .headerSearchPath("./cpp/src/utils"),
                .headerSearchPath("./cpp/src/libs")
            ]
        ),
        .target(
            name: "Inspector",
            dependencies: [
                .product(name: "SQLCipher", package: "SQLCipher.swift")
            ],
            path: "cpp/src",
            exclude: ["libs/CPPLINT.cfg"],
            cxxSettings: [
                .headerSearchPath("utils"),
                .headerSearchPath("libs")
            ],
            linkerSettings: [
                .linkedLibrary("z")
            ]
        )
    ],
    cxxLanguageStandard: .cxx20
)
