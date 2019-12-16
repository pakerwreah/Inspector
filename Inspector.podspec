Pod::Spec.new do |spec|
  spec.name         = "Inspector"
  spec.version      = "1.2.0"
  spec.summary      = "Library to inspect databases in mobile apps"
  spec.homepage     = "https://gitlab.com/paker_wreah/Inspector/Inspector"
  spec.authors      = { "Carlos César Neves Enumo" => "paker_wreah@hotmail.com" }

  spec.source       = { :git => "git@gitlab.com:paker_wreah/Inspector/Inspector.git" }
  spec.source_files = 'cpp/**/*.{h,hpp,c,cpp}', 'ios/Inspector/IOSInspector.{h,mm}'
  spec.public_header_files  = 'ios/Inspector/IOSInspector.h'
  spec.library = 'z'

  spec.dependency 'SQLCipher'
end
