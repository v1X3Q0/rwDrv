# rw_char

a lot of this cmake file was constructued per some sample code here:
`https://gitlab.com/christophacham/cmake-kernel-module/-/blob/master/CMakeLists.txt`

when you are compiling in the simplest case:

```
cmake ..
make
```

In this case, you have linux-headers-\$(uname -r)

There are a couple macros that need attention though, some kernels don't have the symbol `printk` but rather `_printk` which is the basis for my info leak. So for those we append to the cmake `-DLINUX_PRINTK_PREFIX=1`.

Then, some of the `6.x.x+` kernels have different requirements for character device driver types. To use the correct types for those, we use `-DMYCHARCONST=1`.

## Cross Compiling

When you are trying to say cross compile a kernel that you built with linaro as follows:

```
export PREFIX=~/gcc-linaro-x.x.x-x86_64_arm-linux-gnueabi
export PATH="$PREFIX/bin:$PATH"
export ARCH=arm
export KERNEL_BUILD=out
export KERNEL_CONF=defconfig
export KERNEL_ARCH=$ARCH
export TARGET=arm-linux-gnueabi
make ARCH=$KERNEL_ARCH O=$KERNEL_BUILD $KERNEL_CONF
cd $KERNEL_BUILD
make ARCH=$KERNEL_ARCH CROSS_COMPILE=$TARGET- V=1 vmlinux modules
```

sample for cross compiling:

```
cmake .. -DKERNEL_HEADERS_PATH=~/linux-x.x.x/out/
make ARCH=$KERNEL_ARCH CROSS_COMPILE=$TARGET-
```
