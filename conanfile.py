from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout

class OpenPressoCtl(ConanFile):
    name = "openpresso-ctl"
    description = "CLI client for the OpenpressoDaemon gRPC service"
    homepage = "https://openpresso.org"
    url = "https://github.com/openpresso/openpresso-ctl.git"
    license = "GPL-3.0-or-later"
    settings = "os", "arch", "compiler", "build_type"
    package_type = "application"
    exports_sources = "CMakeLists.txt", "src/*"

    def requirements(self):
        self.requires("openpresso_proto/1.0.0-rc2")
        self.requires("cli11/[>=2.3.0]")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        CMake(self).install()
