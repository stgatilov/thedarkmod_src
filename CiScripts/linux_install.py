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
#    os.system('sudo apt-get -y install mesa-common-dev:i386 libglu1-mesa-dev:i386 libglu1-mesa:i386 libgl1-mesa-dev:i386 libgl1:i386 libgl1-mesa-glx:i386 libglvnd-dev:i386')
    os.system('sudo apt-get -y install libxcb-*-dev:i386 libx11-xcb-dev:i386 libxxf86vm-dev:i386 libxext-dev:i386')
    os.system('sudo apt-get -y install libfontenc-dev:i386 libice-dev:i386 libxt-dev:i386 libxaw7-dev:i386 libxcursor-dev:i386 libxfixes-dev:i386 libxdamage-dev:i386 libxinerama-dev:i386 libxkbfile-dev:i386 libxmu-dev:i386 libxmuu-dev:i386 libxpm-dev:i386 libxrandr-dev:i386 libxrender-dev:i386 libxss-dev:i386 libxv-dev:i386 libxcb-cursor-dev:i386 libsm-dev:i386 uuid-dev:i386')
