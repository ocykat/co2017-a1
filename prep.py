import os
import shutil


PACKAGES = [
    "build-essential" ,
    "gdb"             ,
    "libncurses5-dev" ,
    "libssl-dev"      ,
    "kernel-package"  ,
    "openssl"
]

KERNEL_DIR = os.path.expanduser("~/kernelbuild")


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
    os.system("wget https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.4.56.tar.xz")
    os.system("tar -xvJf linux-4.4.56.tar.xz")

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
