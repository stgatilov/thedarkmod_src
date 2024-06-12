import os, platform, sys

def execute(cmd):
    print("CMD: " + cmd)
    assert os.system(cmd) == 0


sysname = platform.system().lower()
if 'windows' not in sysname:
    os.system('sudo apt-get update && sudo apt-get -y install mesa-common-dev libxxf86vm-dev libxext-dev')
