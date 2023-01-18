from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout


class MultiverseConan(ConanFile):
    name = "multiverse"
    version = "0.0.1"
    requires = "boost/1.81.0", "fmt/5.3.0", "ms-gsl/4.0.0"
    generators = "cmake_find_package_multi"

    # Optional metadata
    license = "MIT"
    author = "Dreamsmith Workshop dreamsmithworkshop@gmail.com"
    url = "https://github.com/dreamsmith-workshop/multiverse"
    description = "Libraries and tools resulting from, and supporting, Dreamsmith Workshop business applications."
    topics = ("libraries", "common", "core", "http")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {
        "shared": False,
        "fPIC": True,
        "boost:header_only": True,
        "fmt:header_only": True
    }

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "mltvrs/*", "cmake/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["multiverse"]
