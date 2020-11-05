from conans import ConanFile

class EcFeedConan(ConanFile):
   name = "ecfeed"
   version = "1.0"
   license = "EPL-1.0"
   url = "https://github.com/ecfeed/ecfeed.cpp"
   author = "ecFeed AS (mail@ecfeed.com)"
   description = "An open library used to connect to the ecFeed service. It can be integrated with the most common testing frameworks and generates a stream of test cases using a selected algorithm (e.g. Cartesian, N-Wise)."
   topics = ("test", "ecfeed", "nwise")
   build_policy = 'missing'
   requires = "libcurl/7.72.0", "openssl/1.1.1c"

   def source(self):
      self.run("git clone --single-branch --branch conancenter git@github.com:ecfeed/ecfeed.cpp.git")

   def package(self):
      self.copy("*.hpp", dst="include", keep_path=False)

   def package_id(self):
      self.info.header_only()
