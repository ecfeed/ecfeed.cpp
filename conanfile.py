# from conans import ConanFile, CMake, tools

# class EcfeedConan(ConanFile):
#     name = "ecfeed"
#     version = "0.1"
#     # license = "EPL-1.0"
#     # author = "ecFeed AS mail@ecfeed.com"
#     # url = "https://github.com/ecfeed/ecfeed.cpp"
#     # description = "An open library used to connect to the ecFeed service. It can be integrated with the most common testing frameworks and generates a stream of test cases using a selected algorithm (e.g. Cartesian, N-Wise)."
#     # topics = ("test", "ecfeed", "nwise")
#     settings = "os", "compiler", "build_type", "arch"
#     # options = {"shared": [True, False]}
#     # default_options = {"shared": False}
#     # generators = "cmake"
#     exports_sources = "src/*"

#     def build(self):
#         cmake = CMake(self)
#         cmake.configure(source_folder="src")
#         cmake.build()

#     def package(self):
#         self.copy("*.h", dst="include", src="src")
#         # self.copy("*.lib", dst="lib", keep_path=False)
#         # self.copy("*.dll", dst="bin", keep_path=False)
#         # self.copy("*.so", dst="lib", keep_path=False)
#         # self.copy("*.dylib", dst="lib", keep_path=False)
#         self.copy("*.a", dst="lib", keep_path=False)

#     def package_info(self):
#         self.cpp_info.libs = ["ecfeed"]

from conans import ConanFile


class EcFeedConan(ConanFile):
    name = "ecfeed"
    version = "0.1"
    build_policy = 'missing'
    no_copy_source = True
    exports_sources = "src/*"

    def package(self):
        self.copy("*.hpp", dst="include", keep_path=False)

    def package_id(self):
        self.info.header_only()