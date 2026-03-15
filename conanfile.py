from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps


class SqliteCppRecipe(ConanFile):
    name = "sqlitecpp"
    version = "0.1.0"

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "build_tests": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "build_tests": True,
    }

    # Sources
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "tests/*", "tools/*"

    def requirements(self):
        pass  # No runtime dependencies for now

    def build_requirements(self):
        if self.options.build_tests:
            self.test_requires("gtest/1.14.0")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.variables["SQLITECPP_BUILD_TESTS"] = self.options.build_tests
        # Don't generate CMakeUserPresets.json - we manage our own presets
        tc.user_presets_path = False
        tc.generate()

    def package_info(self):
        self.cpp_info.libs = ["sqlitecpp"]
