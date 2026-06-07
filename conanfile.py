from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
import re

CONAN_TO_DEB_ARCH = {
    "armv8":   "arm64",
    "armv7hf": "armhf",
    "armv7":   "armhf",
    "armv6":   "armhel",
    "x86_64":  "amd64",
    "x86":     "i386",
}

class OpenPressoCtl(ConanFile):
    name = "openpresso-ctl"
    description = "CLI client for the OpenpressoDaemon gRPC service"
    homepage = "https://openpresso.org"
    url = "https://github.com/openpresso/openpresso-ctl.git"
    license = "GPL-3.0-or-later"
    settings = "os", "arch", "compiler", "build_type"
    package_type = "application"
    exports_sources = "CMakeLists.txt", "src/*"
    options = { 
        "build_docs": [True, False, "DocsOnly"]
    }
    default_options = { 
        "build_docs": False
    }

    def set_version(self):
        if not self.version:
            self.version = "0.0.0-unknown"

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.24]")
        if self.options.build_docs:
            self.tool_requires("doxygen/[>=1.16.0 <1.17.0]")

    def requirements(self):
        if(self.options.build_docs != "DocsOnly"):
            self.requires("openpresso_proto/1.0.0-rc2")
            self.requires("cli11/[>=2.3.0]")

    def generate(self):
        major, minor, patch = self.__version_components()
        tc = CMakeToolchain(self)

        tc.cache_variables["CPACK_DEBIAN_PACKAGE_ARCHITECTURE"] = CONAN_TO_DEB_ARCH[str(self.settings.arch)]
        tc.cache_variables["OPENPRESSO_CTL_VERSION"] = self.version
        tc.cache_variables["OPENPRESSO_CTL_VERSION_MAJOR"] = str(major)
        tc.cache_variables["OPENPRESSO_CTL_VERSION_MINOR"] = str(minor)
        tc.cache_variables["OPENPRESSO_CTL_VERSION_PATCH"] = str(patch)
        tc.cache_variables["DOCS_ONLY"] = self.options.build_docs == "DocsOnly"
        tc.cache_variables["OPENPRESSO_CTL_REPO_CHANNEL"] = self.__repo_channel()
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

    def __repo_channel(self) -> str:
        release_pattern = r'\d+\.\d+\.\d+(?:-rc\d+)?'
        canary_pattern = r'\d+\.\d+\.\d+-\d+-g[0-9a-f]{7}'
        if re.fullmatch(release_pattern, self.version):
            return "stable"
        if re.fullmatch(canary_pattern, self.version):
            return "canary"
        return "testing"

    def __version_components(self):
        try:
            pattern = r'(?P<major>\d+)\.(?P<minor>\d+)\.(?P<patch>\d+)'
            match = re.match(pattern, self.version)
            return int(match.group("major")), int(match.group("minor")), int(match.group("patch"))
        except Exception:
            return 0, 0, 0
