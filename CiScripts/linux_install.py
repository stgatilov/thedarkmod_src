import os, platform, sys

def execute(cmd):
    print("CMD: " + cmd)
    assert os.system(cmd) == 0


sysname = platform.system().lower()
if 'windows' not in sysname:
    # this is enough for 64 bit 
    os.system('sudo apt-get update')
    os.system('sudo apt-get -y install mesa-common-dev libglu1-mesa-dev xorg-dev libxcb-*-dev libx11-xcb-dev libxxf86vm-dev libxext-dev')
    # for 32 bit
    os.system('sudo dpkg --add-architecture i386')
    os.system('sudo apt-get update')
    os.system('sudo apt-get -y install gcc-multilib g++-multilib')
    os.system('sudo apt-get -y install mesa-common-dev:i386 libglu1-mesa-dev:i386')
    os.system('sudo apt-get -y install libxcb-*-dev:i386 libx11-xcb-dev:i386 libxxf86vm-dev:i386 libxext-dev:i386')
