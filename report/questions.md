* Why do we need to install `kernel-package`?

The `kernel-package` package includes dependencies required to build the kernel. More information can be found [here](https://launchpad.net/ubuntu/bionic/+package/kernel-package).

* Why do we have to use another kernel source from the server such as `http://www.kernel.org`, can we just compile the original kernel (the local kernel on the running OS) directly?

Instead of downloading and compiling a new kernel, we can compile the original kernel on our machine directly. However, there are certain advantages of building a new kernel version:

* A new kernel version often comes with bug fixes for previous kernel versions.
* A new kernel version is often more stable and faster than the the previous versions.
* A new kernel is better-compatible with certain hardware, such as GPU, wifi card, etc..
* A new kernel version often offers new features and functionalities.

* What is the meaning of other components, i.e. i386, procmem, and sys_procmem?


* What is the meaning of each line above?



* What is the meaning of these two stages, namely “make” and “make modules”?



* Why could this program indicate whether our system call works or not?



* Why do we have to re-define proc_segs struct while we have already defned it inside the kernel?



* Why is root privilege (e.g. adding sudo before the cp command) required to copy the header fle to /usr/include?

In Linux, a normal user only has the write privilege inside his `home` directory. To have write privilege to directories outside `home`, one needs `sudo` to run the command with the superuser's privilege.


* Why must we put -shared and -fpic options into the gcc command?

The `-shared` and `-fPIC` (or `-fpic`) flags are required to build a shared object for dynamic linking. More precisely, according to [here](https://gcc.gnu.org/onlinedocs/gcc-2.95.2/gcc_2.html#SEC13):

`-shared`: Produce a shared object which can then be linked with other objects to form an executable. Not all systems support this option. You must also specify `-fpic' or `-fPIC' on some systems when you specify this option.

`-fPIC`: Emit position-independent code, suitable for dynamic linking and avoiding any limit on the size of the global offset table.