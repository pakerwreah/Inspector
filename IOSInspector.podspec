Pod::Spec.new do |spec|
    spec.name                   = 'IOSInspector'
    spec.version                = '1.5.3'
    spec.license                = 'MIT'
    spec.summary                = 'Library to inspect database and network in mobile apps'
    spec.homepage               = 'https://github.com/pakerwreah/Inspector'
    spec.authors                = { 'Carlos César Neves Enumo' => 'paker_wreah@hotmail.com' }

    spec.source                 = { :git => 'https://github.com/pakerwreah/Inspector.git', :tag => 'v1.5.3' }
    spec.xcconfig               = { 'CLANG_CXX_LANGUAGE_STANDARD' => 'c++17' }
    spec.source_files           = 'cpp/src/**/*.{h,hpp,c,cpp}', 'ios/Inspector/lib/*.{h,mm}'
    spec.public_header_files    = 'ios/Inspector/lib/IOSInspector.h'
    spec.library                = 'z'
    spec.platform               = :ios

    spec.dependency 'SQLCipher'
end
