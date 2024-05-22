from conan import ConanFile
from conan.tools import files
import yaml
from os import path


def get_platform_name(settings, shared=False):
    os = {'Windows': 'win', 'Linux': 'lnx'}[str(settings.os)]
    bitness = {'x86': '32', 'x86_64': '64'}[str(settings.arch)]
    dynamic = 'd' if shared else 's'
    compiler = {'msvc': 'vc', 'gcc': 'gcc'}[str(settings.compiler)]
    # GCC 5-10 are binary compatible, MSVC 2015-2019 are compatible too
    # see also: https://forums.thedarkmod.com/index.php?/topic/20940-unable-to-link-openal-during-compilation/
    ### if compiler in ['vc', 'gcc']:
    ###     compiler += str(settings.compiler.version)
    buildtype = {'Release': 'rel', 'Debug': 'dbg', 'RelWithDebInfo': 'rwd'}[str(settings.build_type)]
    stdlib = '?'
    if compiler.startswith('vc'):
        stdlib = 'm'
        stdlib += {'static': 't', 'dynamic': 'd'}[str(settings.compiler.runtime)]
        stdlib += {'Release': '', 'Debug': 'd'}[str(settings.compiler.runtime_type)]
    elif compiler.startswith('gcc'):
        stdlib = {'libstdc++': 'stdcpp'}[str(settings.compiler.libcxx)]
    return '%s%s_%s_%s_%s_%s' % (os, bitness, dynamic, compiler, buildtype, stdlib)


class TdmDepends(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "with_header": [True, False],
        "with_release": [True, False],
        "with_vcdebug": [True, False],
        "platform_name": [None, "ANY"],
    }
    default_options = {
        "with_header": True,
        "with_release": True,
        "with_vcdebug": True,
        "platform_name": None,
    }

    def set_requirements(self, only_require):
        with open("packages.yaml", "r") as f:
            doc = yaml.safe_load(f)

        for dep in doc["packages"]:
            pkgname = dep["name"]
            optname = "with_" + dep["type"]
            if pkgname == 'xorg' and self.settings.os == "Windows":
                continue      # Linux-only

            if getattr(self.options, optname):
                if only_require:
                    ref = dep["name"] + '/' + dep["version"]
                    if dep["local"]:
                        ref += "@thedarkmod"
                    self.requires(ref, force = True)
                else:
                    options = doc["options"].get(pkgname, {})
                    for k,v in options.items():
                        print("OPT %s = %s on %s" % (k, v, pkgname))
                        setattr(self.options[pkgname], k, v)

    def requirements(self):
        self.set_requirements(True)
    def configure(self):
        self.set_requirements(False)

    def generate(self):
        if str(self.options.platform_name) == "None":
            self.options.platform_name = get_platform_name(self.settings, False)
        platform = self.options.platform_name

        for dep in self.dependencies.values():
            name = str(dep).split('/')[0]
            pkgdir = dep.package_folder
            artdir = path.abspath("..")

            print("{} -> {}".format(pkgdir, path.join(artdir, "artefacts/%s/lib/%s" % (name, platform))))
            # note: we assume recipes are sane, and the set of headers does not depend on compiler/arch
            files.copy(self, "*.h"  , src = path.join(pkgdir, "include") , dst = path.join(artdir, "artefacts/%s/include" % name))
            files.copy(self, "*.H"  , src = path.join(pkgdir, "include") , dst = path.join(artdir, "artefacts/%s/include" % name))    # FLTK =(
            files.copy(self, "*.hpp", src = path.join(pkgdir, "include") , dst = path.join(artdir, "artefacts/%s/include" % name))
            files.copy(self, "*"    , src = path.join(pkgdir, "licenses"), dst = path.join(artdir, "artefacts/%s/licenses" % name))
            # source code files to be embedded into build (used by Tracy)
            files.copy(self, "*.cpp", src = path.join(pkgdir, "src")    , dst = path.join(artdir, "artefacts/%s/src" % name))
            files.copy(self, "*.c"  , src = path.join(pkgdir, "src")    , dst = path.join(artdir, "artefacts/%s/src" % name))
            # compiled binaries are put under subdirectory named by build settings
            files.copy(self, "*.lib", src = path.join(pkgdir, "lib")    , dst = path.join(artdir, "artefacts/%s/lib/%s" % (name, platform)))
            files.copy(self, "*.a"  , src = path.join(pkgdir, "lib")    , dst = path.join(artdir, "artefacts/%s/lib/%s" % (name, platform)))
            # while we don't use dynamic libraries, some packages provide useful executables (e.g. FLTK gives fluid.exe)
            files.copy(self, "*.dll", src = path.join(pkgdir, "bin")    , dst = path.join(artdir, "artefacts/%s/bin/%s" % (name, platform)))
            files.copy(self, "*.so" , src = path.join(pkgdir, "bin")    , dst = path.join(artdir, "artefacts/%s/bin/%s" % (name, platform)))
            files.copy(self, "*.exe", src = path.join(pkgdir, "bin")    , dst = path.join(artdir, "artefacts/%s/bin/%s" % (name, platform)))
            files.copy(self, "*.bin", src = path.join(pkgdir, "bin")    , dst = path.join(artdir, "artefacts/%s/bin/%s" % (name, platform)))
