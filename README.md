# Inspector
[![release](https://img.shields.io/github/v/release/pakerwreah/Inspector)](https://github.com/pakerwreah/Inspector/releases/latest)
![c++](https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B)
![android](https://img.shields.io/badge/Android-grey.svg?style=flat&logo=android)
![ios](https://img.shields.io/badge/iOS-grey.svg?style=flat&logo=apple)
[![codecov](https://img.shields.io/codecov/c/gh/pakerwreah/Inspector?label=codecov&logo=codecov)](https://codecov.io/gh/pakerwreah/Inspector)
[![ci](https://github.com/pakerwreah/Inspector/workflows/Unit%20Tests/badge.svg)](https://github.com/pakerwreah/Inspector/actions)
[![ci](https://github.com/pakerwreah/Inspector/workflows/Android%20CI/badge.svg)](https://github.com/pakerwreah/Inspector/actions)
[![ci](https://github.com/pakerwreah/Inspector/workflows/iOS%20CI/badge.svg)](https://github.com/pakerwreah/Inspector/actions)

### Tool to inspect SQLite databases and intercept network requests from mobile applications.

Project targeting Android, iOS and Web using C/C++, Java, Objective-C, Vue.js, WebSocket, SQLite and IndexedDB.
The data is shown in a web interface and uses WebSocket to communicate directly with the app via local network.
To keep network log history the web system uses the built-in IndexedDB from the browser.

#### Frontend repository
https://github.com/pakerwreah/InspectorWeb

## Android
[![Version](https://jitpack.io/v/pakerwreah/Inspector.svg)](https://jitpack.io/#pakerwreah/Inspector)

<details>
    <summary>Setup</summary>

#### Gradle
```gradle
repositories {
    maven { url "https://jitpack.io" }
}
```
```gradle
dependencies {
    implementation "com.github.pakerwreah:Inspector:<release-tag>"
}
```

#### Proguard
```
-keep class br.newm.inspector.* { *; }
```
</details>

<details>
    <summary>Usage</summary>

#### Application
```java
import br.newm.inspector.Inspector;

public class Application extends android.app.Application {
    @Override
    public void onCreate() {
        super.onCreate();

        Inspector.initializeWith(this);

        // SQLCipher support
        // database name, password, sqlcipher major version
        Inspector.setCipherKey("database_cipher3.db", "123456", 3);
        Inspector.setCipherKey("database_cipher4.db", "123456", 4);
    }

    // Optional: if you want to specify databases names to show
    @Override
    public String[] databaseList() {
        return new String[]{"database.db", "database_cipher3.db", "database_cipher4.db"};
    }
}
```

#### Intercept network requests
<sub>:warning: if you use addNetworkInterceptor it won't intercept timeouts</sub>
```java
import br.newm.inspector.NetworkInterceptor;

new OkHttpClient.Builder().addInterceptor(new NetworkInterceptor());
```

#### Static plugins
Accepts returning JSON, HTML or plain text
```java
Inspector.addPlugin("prefs", "Shared Preferences", new PluginAction() {
    @Override
    public String action() {
        return new JSONObject(prefs.getAll()).toString();
    }
});
```

#### Live plugins
Accepts complex HTML frontend with javascript support
<br />
<sub>Check ExplorerPlugin.java for a full example</sub>
```java
Inspector.addLivePlugin("explorer", "Explorer", new PluginAction() {
    @Override
    public String action() {
        // return plugin frontend
    }
});
```

#### Plugin API
Route with parameters to be used as a plugin or standalone api
<br />
<sub>Check ExplorerPlugin.java for a full example</sub>
```java
Inspector.addPluginAPI("GET", "filesystem/list", new PluginAPIAction() {
    @Override
    public String action(Map<String, String> params) {
        // return json array with list of files
    }
});

Inspector.addPluginAPI("GET", "filesystem/open", new PluginAPIActionBinary() {
    @Override
    public byte[] action(Map<String, String> params) {
        // return file contents
    }
});
```

#### Websockets
Send messages to your live plugins
```javascript
new WebSocket(`ws://${location.hostname}:${location.port}/plugins/ws/mykey`)
```
```java
Inspector.sendMessage("mykey", "Hello world!");
```
#
:warning: You should run this command to work with emulators
```bash
# same port number used to initialize the plugin
adb forward tcp:30000 tcp:30000
```
Or configure its network as bridge and use the device's IP
</details>

## iOS
[![Version](https://img.shields.io/cocoapods/v/IOSInspector.svg)](https://cocoapods.org/pods/IOSInspector)

<details>
    <summary>Setup</summary>

#### CocoaPods
```gradle
target 'MyApp' do
   pod "IOSInspector"
end
```
</details>

<details>
    <summary>Usage</summary>

#### AppDelegate
```swift
@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate, IOSInspectorProtocol {

    func applicationDidFinishLaunching(_ application: UIApplication) {

        IOSInspector.initialize(withDelegate: self)

        // SQLCipher support
        IOSInspector.setCipherKey("database_cipher3.db", password: "123456", version: 3)
        IOSInspector.setCipherKey("database_cipher4.db", password: "123456", version: 4)
    }

    // Required: specify databases paths
    func databaseList() -> [String] {
        let documentsPathURL = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first!

        return ["database.db", "database_cipher3.db", "database_cipher4.db"].map {
            documentsPathURL.appendingPathComponent($0).absoluteString
        }
    }
}
```

#### Intercept network requests
```swift
let uid = UUID().uuidString

let request = URLRequest(url: url)

// send request to the frontend
IOSInspector.sendRequest(withUID: uid, request: request)

URLSession.shared.dataTask(with: request) { data, response, error in

    if let data = data, let response = response as? HTTPURLResponse {
        // send response to the frontend
        IOSInspector.sendResponse(withUID: uid, response: response, body: data)
    }

}.resume()
```

#### Static plugins
Accepts returning JSON, HTML or plain text
```swift
IOSInspector.addPlugin("prefs", name: "User Defaults") {
    let dict = UserDefaults.standard.dictionaryRepresentation()
    if let data = try? JSONSerialization.data(withJSONObject: dict),
        let json = String(data: data, encoding: .utf8) {
        return json
    }
    return "No data"
}
```

#### Live plugins
Accepts complex HTML frontend with javascript support
<br />
<sub>Check ExplorerPlugin.swift for a full example</sub>
```swift
IOSInspector.addLivePlugin("explorer", name: "Explorer") {
    // return plugin frontend
}
```

#### Plugin API
Route with parameters to be used as a plugin or standalone api
<br />
<sub>Check ExplorerPlugin.swift for a full example</sub>
```swift
IOSInspector.addPluginAPI(forMethod: "GET", path: "filesystem/list") { params -> String in
    // return json array with list of files
}

IOSInspector.addPluginAPI(forMethod: "GET", path: "filesystem/open") { params -> Data? in
    // return file contents
}
```

#### Websockets
Send messages to your live plugins
```javascript
new WebSocket(`ws://${location.hostname}:${location.port}/plugins/ws/mykey`)
```
```swift
IOSInspector.sendMessage(to: "mykey", message: "Hello world!")
```
</details>
