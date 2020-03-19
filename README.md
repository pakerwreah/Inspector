# Inspector
Tool to inspect SQLite databases and intercept network requests from mobile applications.
___

Project targeting Android, iOS and Web using C/C++, Java, Objective-C, Vue.js, WebSocket, SQLite and IndexedDB.
The data is shown in a web interface and uses WebSocket to communicate directly with the app via local network.
To keep network log history the web system uses the built-in IndexedDB from the browser.

#### Frontend repository
https://github.com/pakerwreah/InspectorWeb

## Android

#### Gradle
```gradle
repositories {
    maven {
        url 'https://artifactory.newm.com.br/artifactory/inspector'
    }
}
// ...
dependencies {
    implementation "br.newm.inspector:inspector:1.2.2"
}
```

#### Proguard
```
-keep class br.newm.inspector.* { *; }
```

#### Application
```java
import br.newm.inspector.Inspector;

public class Application extends android.app.Application {
    @Override
    public void onCreate() {
        super.onCreate();

        // context, port
        Inspector.initializeWith(this, 30000);
        
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
```java
import br.newm.inspector.NetworkInterceptor;

new OkHttpClient.Builder().addNetworkInterceptor(new NetworkInterceptor());
```

## iOS

#### CocoaPods
```
pod "Inspector", :git => 'git@github.com:pakerwreah/Inspector.git'
```

#### AppDelegate
```swift
@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate, IOSInspectorProtocol {

    func applicationDidFinishLaunching(_ application: UIApplication) {
        
        IOSInspector.initialize(withDelegate: self, port: 30000)

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