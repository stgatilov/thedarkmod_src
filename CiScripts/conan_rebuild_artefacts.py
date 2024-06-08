import os, shutil

def execute(cmd):
    print("CMD: " + cmd)
    assert os.system(cmd) == 0


print('CONAN_HOME = ' + os.environ['CONAN_HOME'])

os.chdir('../ThirdParty')

shutil.rmtree('artefacts', ignore_errors = True)
execute('python ./1_export_custom.py --unattended')
execute('python ./2_build_all.py --unattended')

os.chdir('../CiScripts')
