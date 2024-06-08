import os, platform, sys

def execute(cmd):
    print("CMD: " + cmd)
    assert os.system(cmd) == 0

bitness = sys.argv[1]
projects = {
    'game': '..',
    'installer': '../tdm_installer',
    'packager': '../tdm_package',
}

sysname = platform.system().lower()
if 'windows' in sysname:
    platform = {'32': 'Win32', '64': 'x64'}[bitness]

    for proj,cmakedir in projects.items():
        execute(f'cmake {cmakedir} -B {proj}_win_{bitness} -A {platform} -DCOPY_EXE=OFF')
        execute(f'cmake --build {proj}_win_{bitness} --config Release')
        execute(f'cmake --build {proj}_win_{bitness} --config Debug')

else:
    numcores = os.cpu_count()

    for proj,cmakedir in projects.items():
        if bitness == '32':
            path = os.path.relpath('../sys/cmake/gcc_32bit.cmake', cmakedir)
            toolchain = f'-DCMAKE_TOOLCHAIN_FILE={path}'
        elif bitness == '64':
            toolchain = ''

        execute(f'cmake {cmakedir} -B {proj}_linux_{bitness}_rel -DCOPY_EXE=OFF -DCMAKE_BUILD_TYPE=Release {toolchain}')
        execute(f'cmake --build {proj}_linux_{bitness}_rel -j {numcores}')
        execute(f'cmake {cmakedir} -B {proj}_linux_{bitness}_dbg -DCOPY_EXE=OFF -DCMAKE_BUILD_TYPE=Debug {toolchain}')
        execute(f'cmake --build {proj}_linux_{bitness}_dbg -j {numcores}')
