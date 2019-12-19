# Inspector

Android:

```gradle
repositories {
    maven {
        url 'https://artifactory.newm.com.br/artifactory/inspector'
    }
}

dependencies {
    implementation "br.newm.inspector:inspector:1.2.1"
}
```

Proguard:
```
-keep class br.newm.inspector.* { *; }
```
