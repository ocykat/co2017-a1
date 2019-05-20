# Assignment

## Requirement
Implement system call `procmem`. The output of this system call is the information of the memory layout of a process.

```
Code Segment start = 0x8048000, end = 0x809fc38
Data Segment start = 0x80a0000, end = 0x80a0ec4
Stack Segment start = 0xbffffb30
```

The output of a process should match the content of the file `/proc/<pid>/maps`.


## My Virtual Machine
|     Operating System     | Ubuntu 18.04 LTS (Bionic Beaver) - 64bit |
| Linux Version (original) |                  4.15.0                  |
| Virtual Machine Software |         Oracle Virtual Box 6.0.6         |
|          Storage         |       32GB (with 4GB for swap area)      |

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

```sh
Makefile:976: "Cannot use CONFIG_STACK_VALIDATION=y, please install libelf-dev, libelf-devel or elfutils-libelf-devel"
```

From Google, I found that this is likely a bug in Linux. If you're using Ubuntu 18.04 like me when I worked on this project, you may want to install an additional package:

```sh
sudo apt install libelf-dev
```

After building, information about the module can be read by running:

```sh
modinfo hello-1.ko
```

Insert the module into the kernel:

```sh
sudo insmod ./hello-1.ko
```

To check if the module has been successfully inserted to the kernel, run either of these two commands:

```sh
lsmod
cat /proc/modules
```

The module with the name `hello-1` should appear at the top.


Finally, remove the module from the kernel and recheck:

```sh
sudo rmmod hello-1
```

To check if the module really prints anything at all, use the command

```sh
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
...
static int myint;

static int __init my_init_func(void)  { ... }
static void __exit my_exit_func(void) { ... }

module_init(my_init_func);
module_exit(my_exit_func);
module_param(myint1, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
module_param(myint2, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
```

It is a good idea (IMO) to give **write and read** permission to the owner of the module, and **read-only** permission to other users.

To pass command line arguments, the syntax is:
```
insmod modulename.ko myint1=10 myint2=20
```

[More info (1)](http://www.tldp.org/LDP/lkmpg/2.6/html/x323.html)

[More info (2)](https://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html).


### Data Structures
* `task_struct`: Each process running under a Linux machine is associated with a struct `task_struct`. The source code of `task_struct` can be read [here](https://github.com/torvalds/linux/blob/a13f0655503a4a89df67fdc7cac6a7810795d4b3/include/linux/sched.h#L585).

* `mm_struct`: Inside a `task_struct`, there is a struct `mm_struct` which stores the description of memory of the process. The source code of `mm_struct` can be read [here](https://github.com/torvalds/linux/blob/a13f0655503a4a89df67fdc7cac6a7810795d4b3/include/linux/mm_types.h#L351).

In this assignment, we are interested in these fields:
```
└── struct task_struct
    └── struct mm_struct
        ├── unsigned long start_code;  => where Code Segment starts
        ├── unsigned long end_code;    => where Code Segment ends
        ├── unsigned long start_data;  => where Data Segment starts
        ├── unsigned long end_data;    => where Data Segment ends
        └── unsigned long start_stack; => where Stack starts
```

### Macro: `find_all_process`
#### Introduction and Usage
The `find_all_process` macro is used to traverse through all running process in the system.

#### Big Warning (MUST READ for new kernel versions)
The `find_all_process` macro used to locate in the `<linux/sched.h>` header, but it is no longer the case for the newer versions of the Linux kernel: its location has changed to the `<linux/sched/signal.h>` header since [this commit](https://github.com/torvalds/linux/commit/c3edc4010e9d102eb7b8f17d15c2ebc425fed63c). The best way to check where the macro locates is to go to the header directory of the kernel and use a tool like `grep` to check.

A simple `grep` command to find the word `pattern` in a directory `/path/to/somewhere/` is:

```sh
grep -rnw '/path/to/somewhere/' -e 'pattern'
```

If you want to find the header file where the macro `find_all_process` locates, you will need something like this:

```sh
# go to wherever your kernel header files locate
cd /usr/src/linux-headers-4.15.0-48/include/linux
# run grep
grep -rnw . -e 'find_all_process'
```

The result should be something like this:

```sh
./sched/signal.h:503:#define for_each_process(p) \
./sched/signal.h:526:	for_each_process(p) for_each_thread(p, t)
```

For this version, you must include `<linux/sched/signal.h>` if you want to use `find_all_process`.


This means that the `find_all_process` macro locates in the `<linux/sched/signal.h>` for version 4.15.0-48.

For an older version, in particular, version 4.4.56, the result is different:

```sh
./sched.h:2663:#define for_each_process(p) \
./sched.h:2686:	for_each_process(p) for_each_thread(p, t)
```

For this version, you must include `<linux/sched.h>` if you want to use `find_all_process`.

### Function: `copy_to_user`
This function copy data from kernel-space to user-space.

[More info](https://www.quora.com/Linux-Kernel-How-does-copy_to_user-work/answer/Robert-Love-1)

### Building Kernel

For a full list of dependencies to build the Linux kernel, see [here](https://wiki.ubuntu.com/KernelTeam/GitKernelBuild).


## Steps for doing this assignment
* **Step 1**: Prepare: Run the `prep.py` file. This file does the following things:

* * **Step 1.1**: Download all required packages.

* * **Step 1.2**: Create a directory `~/kernelbuild` where the kernel is built. We call this directory the **build directory**.

* * **Step 1.3**: Download the source code of the Linux kernel to the build directory. The version used for this project is `4.4.56`.

* * **Step 1.4**: Copy the configuration file `.config` of the existing kernel into the build directory. The `.config` file of the existing kernel is located in the `/boot` directory.

* * **Step 1.5**: (Optional) You can run `sudo menuconfig` to recheck the configuration again before building the kernel.

* **Step 2**: Add the source code of the system call to the build directory.

* * **Step 2.1**: Open the file `include/linux/syscalls.h` and add the following lines to the end of that file:

```c
struct proc_segs;
asmlinkage long sys_procmem(int pid, struct proc_segs* info);
```

* * **Step 2.2**: Open the file `arch/x86/kernel/Makefile` and add the following line:

```c
obj-y += sys_procmem.o # name of syscall object file
```

* * **Step 2.3**: Write the source code of the system call in the file `arch/x86/kernel/sys_procmem.c`.

* **Step 3**: Add the system call to the syscall table.
Open the syscall table file. The syscall table file locates in the directory `arch/x86/entry/syscalls`. The file you want to change is either `syscall_32.tbl` or `syscall_64.tbl` depending on whether your machine is 32-bit or 64-bit. To check if your machine is 32-bit or 64-bit, use this command:

```sh
uname -m
```

The result is `i686` or `i386` if you are on a 32-bit machine, and `x86_64` on a 64-bit machine. In my case, the VM is 64-bit. Therefore, I change the file `syscall_64.tbl`.

The description of the file is specified very clearly at the top and you should take a look at it. You should also take a look at the content of the file to know what you should add.

In my case, I add the following line to the file `arch/x86/entry/syscalls/syscall_64.tbl`:

```sh
## My syscall: procmem
444	64	procmem			__x64_sys_procmem
```

* **Step 3**: Build the kernel.

Building the kernel will take very long. One way to reduce building time is to build with multiple threads.

```sh
sudo make -j 4
sudo make -j 4 modules
sudo make -j 4 modules_install
sudo make -j 4 install
sudo reboot
```

Explanation for each steps:
* `make`:  compiles and links the kernel image. This is a single file named `vmlinuz` (or `bzImage`, not sure for now).
* `make modules`: compiles individual files for each question you answered `M` during kernel config. The object code is linked against your freshly built kernel. (For questions answered `Y`, these are already part of `vmlinuz`, and for questions answered `N` they are skipped).
* `make install`: installs your built kernel to `/vmlinuz`.
* `make modules_install`: installs your kernel modules to `/lib/modules` or `/lib/modules/<version>`.

*Note*: If there is an error at any steps, check carefully if the code for the system call is correct or not.

[More info](https://unix.stackexchange.com/questions/20864/what-happens-in-each-step-of-the-linux-kernel-building-process)


* **Step 4**: Verify if the new kernel has been installed.

```sh
uname -r
```

* **Step 5**: Write a C wrapper for the system call.
Write a wrapper for the system call. The wrapper in this project is a shared object for dynamic linking. Two files are required: `procmem.c` and `procmem.h`.

* * **Step 5.1**: Copy the header file `procmem.h` to the directory `/usr/include`.

```sh
sudo cp procmem.h /usr/include/
```

* * **Step 5.2**: Compile the `procmem` to a shared object for dynamic linking and copy the shared object file to the directory `/usr/lib`.

```sh
gcc -shared -fpic procmem.c -o libprocmem.so
sudo cp libprocmem.so /usr/lib/
```

*gcc flags*:

`-shared`: Produce a shared object which can then be linked with other objects to form an executable. Not all systems support this option. You must also specify `-fpic' or `-fPIC' on some systems when you specify this option. (*[source](https://gcc.gnu.org/onlinedocs/gcc-2.95.2/gcc_2.html#SEC13)*).

`-fPIC`: Emit position-independent code, suitable for dynamic linking and avoiding any limit on the size of the global offset table.
*More info on position-independent code can be found [here](https://en.wikipedia.org/wiki/Position-independent_code) and [here](https://www.quora.com/When-exactly-should-I-use-GCCs-fPIC-option)*.

* * **Step 5.3**: Write a small C file to test the new shared object.


