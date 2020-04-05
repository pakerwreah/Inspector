Pod::Spec.new do |spec|
  spec.name         = "Inspector"
  spec.version      = "1.3.0"
  spec.summary      = "Library to inspect database and network in mobile apps"
  spec.homepage     = "https://github.com/pakerwreah/Inspector"
  spec.authors      = { "Carlos César Neves Enumo" => "paker_wreah@hotmail.com" }

  spec.source       = { :git => "https://github.com/pakerwreah/Inspector.git", :tag => 'v1.3.0' }
  spec.source_files = 'cpp/**/*.{h,hpp,c,cpp}', 'ios/Inspector/lib/*.{h,mm}'
  spec.public_header_files  = 'ios/Inspector/lib/IOSInspector.h'
  spec.library = 'z'

  spec.dependency 'SQLCipher'
end
