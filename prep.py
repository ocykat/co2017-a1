import os
import shutil

# A list of packages required to build the kernel can be found here:
# https://wiki.ubuntu.com/KernelTeam/GitKernelBuild

PACKAGES = [
    "bison"           ,
    "build-essential" ,
    "ccache"          ,
    "fakeroot"        ,
    "flex"            ,
    "gdb"             ,
    "libncurses5-dev" ,
    "libssl-dev"      ,
    "kernel-package"  ,
    "openssl"
]

KERNEL_DIR = os.path.expanduser("~/kernelbuild2")


def install_package(package):
    os.system("sudo apt install -y {0}".format(package))


def main():
    # Input student id
    while True:
        print("Your student id? -> ", end='')
        STUDENT_ID = input()
        print("Are you sure your student id is [{0}]? Type y or n -> ".format(STUDENT_ID), end='')
        signal = input()
        if signal == 'y':
            break

    # Install packages
    for package in PACKAGES:
        install_package(package)

    # Create directory for kernel
    os.system("mkdir {0}".format(KERNEL_DIR))
    os.chdir(KERNEL_DIR)

    # Download & extract kernel source
    # This is the previous Linux version introduced by the assignment instruction
    # os.system("wget https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.4.56.tar.xz")
    # os.system("tar -xvJf linux-4.4.56.tar.xz")

    # This is the Linux version I want to test
    # - the Linux version for Ubuntu 18.10
    # The .tar.xz file could not be downloaded,
    # so I chose the .tar.gz file instead
    os.system("wget http://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.18.1.tar.gz")
    os.system("gunzip linux-4.18.1.tar.gz")
    os.system("tar xvf linux-4.18.1.tar")

    os.system("ls /boot/ > boot-dir.tmp")

    with open("boot-dir.tmp", "r") as f:
        for line in f:
            if "config-" in line:
                config_file = line[:-1]
                break

    # Copy .config file
    print("Config file: {0}".format(config_file))
    shutil.copyfile("/boot/{0}".format(config_file), "config.tmp")

    # Add student id to .config file
    with open("config.tmp", "r") as inf:
        with open(".config", "w+") as ouf:
            for line in inf:
                if "CONFIG_LOCALVERSION=" in line:
                    ouf.write("CONFIG_LOCALVERSION=.\"{0}\"\n".format(STUDENT_ID))
                else:
                    ouf.write(line)

    # Remove temp files
    os.system("rm boot-dir.tmp")
    os.system("rm config.tmp")


if __name__ == '__main__':
    main()
