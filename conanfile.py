import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, CMakeDeps, cmake_layout


class MultiverseConan(ConanFile):
    name = "mltvrs"
    version = "0.0.1"
    requires = "boost/1.81.0", "fmt/5.3.0", "ms-gsl/4.0.0"
    generators = "CMakeDeps"

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
        tc.cache_variables["CMAKE_CXX_STANDARD"] = 20
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.components["ietf"].set_property(
            "cmake_target_name", "mltvrs_ietf")
        self.cpp_info.components["shop"].set_property(
            "cmake_target_aliases", ["mltvrs::ietf"])

        self.cpp_info.components["shop"].set_property(
            "cmake_target_name", "mltvrs_shop")
        self.cpp_info.components["shop"].set_property(
            "cmake_target_aliases", ["mltvrs::shop"])

        self.cpp_info.set_property(
            "cmake_build_modules",
            [
                os.path.join(
                    "lib",
                    "cmake",
                    "mltvrs",
                    "mltvrs",
                    "project-config.cmake")
            ]
        )
