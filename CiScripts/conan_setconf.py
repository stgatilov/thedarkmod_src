import os

def execute(cmd):
    print("CMD: " + cmd)
    assert os.system(cmd) == 0

execute('conan config install global.conf')
