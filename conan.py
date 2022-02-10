from conans import ConanFile, CMake, tools

def get_version():
    try:
        content = tools.load("VERSION")
        return content.strip()
    except Exception as e:
        return None

class VerumConan(ConanFile):
    name = "verum"
    version = get_version()
    license = "MIT"
    author = "Ming Kai Chen <mingkaichen2009@gmail.com>"
    url = "https://github.com/mingkaic/verum"
    description = "C++ utility packages."
    topics = ["conan", "utility"]
    settings = "os", "compiler", "build_type", "arch"
    requires = (
        "cisab/0.1.0@mingkaic-co/stable",
        "gtest/1.10.0",
    )
    generators = "cmake", "cmake_find_package_multi"

    options = {
        "fPIC": [True, False],
    }
    default_options = {
        "fPIC": True,
    }

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions['CMAKE_POSITION_INDEPENDENT_CODE'] = self.options.fPIC
        cmake.configure()
        return cmake

    def source(self):
        self.run("git clone {}.git .".format(self.url))

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        self.copy(pattern="LICENSE.*", dst="licenses", keep_path=False)
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.names["cmake_find_package"] = self.name
        self.cpp_info.names["cmake_find_package_multi"] = self.name
        self.cpp_info.libs = ["diff", "egrpc", "error", "estd", "flag", "fmts", "logs"]
