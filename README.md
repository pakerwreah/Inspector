# Inspector

Android:

```gradle
dependencies {
    implementation "net.zetetic:android-database-sqlcipher:4.2.0@aar"
    implementation "br.newm.inspector:inspector:1.2.0"
}
```

Proguard:
```
-keep class br.newm.inspector.* { *; }
```
