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
            cxxSettings: [
                .headerSearchPath("../../../cpp/src"),
                .headerSearchPath("../../../cpp/src/utils"),
                .headerSearchPath("../../../cpp/src/libs")
            ]
        ),
        .target(
            name: "Inspector",
            dependencies: ["SQLCipher"], // FIXME: Waiting for official SPM support
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
    cxxLanguageStandard: .cxx1z
)
