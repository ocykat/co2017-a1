# Assignment

## Requirement
Implement system call `procmem`. The output of this system call is the information of the memory layout of a process.

```
Code Segment start = 0x8048000, end = 0x809fc38
Data Segment start = 0x80a0000, end = 0x80a0ec4
Stack Segment start = 0xbffffb30
```

## My Virtual Machine
* OS: Ubuntu 18.04 Bionic Beaver LTS - 64bit version
* VM Software: Oracle Virtual Box

The version of Ubuntu that I installed is a minimal version with no Desktop Environment. The `.iso` file for the minimal version can be downloaded [here](https://help.ubuntu.com/community/Installation/MinimalCD). Also, dotfiles can be found [here]().


## Background Knowledge
### Kernel Modules
#### What is kernel modules?
**Kernel modules** are pieces of code that can be loaded and unloaded into the kernel upon demand. Without modules, one needs to rebuild and reboot the kernel every time a new functionality is added - which is super time-consuming for our testing purpose.

[*More info*](http://tldp.org/LDP/lkmpg/2.6/html/x40.html)

#### Warnings
* A module compiled for one kernel won't load if you boot a different kernel (unless you enable CONFIG_MODVERSIONS in the kernel). [*More info*](http://tldp.org/LDP/lkmpg/2.6/html/x44.html#FTN.AEN62)

#### "Hello World" Module
This is an example of a kernel module:
* `hello-1.c`:
```c
// hello-1.c - The simplest kernel module.
#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */

int init_module(void)
{
    printk(KERN_INFO "Hello world 2.\n");

    // A non 0 return means init_module failed; module can't be loaded.
    return 0;
}

void cleanup_module(void)
{
    printk(KERN_INFO "Goodbye world 1.\n");
}
```
* `Makefile`:
```
obj-m += hello-1.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

Kernel modules must have at least two functions:
* a `start` (initialization) function called `init_module()` which is called when the module is `insmod`ed into the kernel,
* an `end` (cleanup) function called cleanup_module() which is called just before it is `rmmod`ed. 

*Note*: When you run the Makefile, there is a chance that you get this error message.
```
Makefile:976: "Cannot use CONFIG_STACK_VALIDATION=y, please install libelf-dev, libelf-devel or elfutils-libelf-devel" 
```
From Google, I found that this is likely a bug in Linux. If you're using Ubuntu 18.04 like me when I worked on this project, you may want to install an additional package:
```
sudo apt install libelf-dev
```

After building, information about the module can be read by running:
```
modinfo hello-1.ko
```

Insert the module into the kernel:
```
sudo insmod ./hello-1.ko
```

To check if the module has been successfully inserted to the kernel, run either of these two commands:
```
lsmod
cat /proc/modules
```

The module with the name `hello-1` should appear at the top.


Finally, remove the module from the kernel and recheck:
```
sudo rmmod hello-1
```

To check if the module really prints anything at all, use the command
```
dmesg
```
to read the kernel messages. [More info](https://elinux.org/Debugging_by_printing).


#### Init Macros
For newer versions of Linux, the init and cleanup functions do not have to be named `init_module()` and `cleanup_module()` thanks to the `module_init()` and `module_exit()` macros defined in `linux/init.h`.

```c
#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros */

static int __init hello_2_init(void)
{
    printk(KERN_INFO "Hello, world 2\n");
    return 0;
}

static void __exit hello_2_exit(void)
{
    printk(KERN_INFO "Goodbye, world 2\n");
}

module_init(hello_2_init);
module_exit(hello_2_exit);
```

[More info](http://tldp.org/LDP/lkmpg/2.6/html/hello2.html)


#### Passing Command Line Arguments to a Module

The module accepts command line arguments through the `module_param()` macro:
```c
module_param(varname, datatype, permission bits)
```

The variable `varname` must be a global variable in the source file of the module.

For example:

```c
module_param(myint, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
```

More information on permission bits can be found [here](https://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html). It is a good idea (IMO) to give **write and read** permission to the owner of the module, and **read-only** permission to other users.

[More info](http://www.tldp.org/LDP/lkmpg/2.6/html/x323.html)


### Data Structures
* `task_struct`: Each process running under a Linux machine is associated with a struct `task_struct`. The source code of `task_struct` can be read [here](https://github.com/torvalds/linux/blob/a13f0655503a4a89df67fdc7cac6a7810795d4b3/include/linux/sched.h#L585).

* `mm_struct`: Inside a `task_struct`, there is a struct `mm_struct` which stores the description of memory of the process. The source code of `mm_struct` can be read [here](https://github.com/torvalds/linux/blob/a13f0655503a4a89df67fdc7cac6a7810795d4b3/include/linux/mm_types.h#L351).


## Steps for doing this assignment
* **Step 1**: Prepare: Run the `prep.py` file. This file does the following things:
  * **Step 1.1**: Download all required packages.
  * **Step 1.2**: Create a directory `~/kernelbuild` where the kernel is built. We call this directory the **build directory**.
  * **Step 1.3**: Download the source code of the Linux kernel to the build directory. The version used for this project is `4.4.56`.
  * **Step 1.4**: Copy the configuration file `.config` of the existing kernel into the build directory. The `.config` file of the existing kernel is located in the `/boot` directory. 
* **Step 2**: