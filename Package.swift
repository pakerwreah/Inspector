// swift-tools-version:5.3

import PackageDescription

let package = Package(
    name: "IOSInspector",
    products: [
        .library(
            name: "IOSInspector",
            targets: ["IOSInspector"]
        )
    ],
    targets: [
        .target(
            name: "IOSInspector",
            dependencies: ["Inspector"],
            path: "ios/Inspector/lib",
            publicHeadersPath: ".",
            cSettings: [
                .define("NO_SQLCIPHER") // FIXME: Waiting for official SPM support
            ],
            cxxSettings: [
                .headerSearchPath("../../../cpp/src"),
                .headerSearchPath("../../../cpp/src/utils"),
                .headerSearchPath("../../../cpp/src/libs")
            ]
        ),
        .target(
            name: "Inspector",
            path: "cpp/src",
            exclude: ["libs/CPPLINT.cfg"],
            cxxSettings: [
                .headerSearchPath("utils"),
                .headerSearchPath("libs")
            ],
            linkerSettings: [
                .linkedLibrary("z"),
                .linkedLibrary("sqlite3")
            ]
        )
    ],
    cxxLanguageStandard: .cxx20
)
