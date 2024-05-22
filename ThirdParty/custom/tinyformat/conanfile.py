from conan import ConanFile
from conan.tools import files
from os import path

class TinyformatConan(ConanFile):
    name = "tinyformat"
    license = "Boost Software License - Version 1.0. http://www.boost.org/LICENSE_1_0.txt"
    author = "Stepan Gatilov stgatilov@gmail.com"
    description = "A minimal type safe printf() replacement"
    topics = ("format", "printf")

    def source(self):
        files.get(self, **self.conan_data["sources"][self.version], strip_root=True)

    def package(self):
        files.copy(self, "tinyformat.h", src = path.join(self.source_folder), dst = path.join(self.package_folder, "include"))
        files.copy(self, "*LICENSE", src = path.join(self.source_folder), dst = path.join(self.package_folder, "licenses"), keep_path = False)
