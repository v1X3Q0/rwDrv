#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <linux/types.h>

#include <localUtil.h>

#include <krw_util.h>
#include "krw.h"

int fd = { 0 };

int kernel_leak(size_t* leak_out)
{
   return kernel_read(leak_out, sizeof(size_t), LEAK_PRINTK);
}

int kernel_init()
{
   int result = -1;
   fd = open("/dev/" CHARNAME, O_RDWR);
   // fd.fdIn = open("/dev/" CHARNAME "-0", O_RDONLY);
   // fd.fdOut = open("/dev/" CHARNAME "-1", O_WRONLY);

   SAFE_BAIL(fd < 0);

   result = 0;
fail:
   return result;
}

// int kSeek(size_t dst)
// {
//    size_t seekBuf[2] = {0};
//    size_t setState = SEEK_OP;

//    seekBuf[0] = setState;
//    seekBuf[1] = dst;

//    write(fd.fdOut, seekBuf, sizeof(seekBuf));
//    return 0;
// }

int kernel_read(void* buf, size_t len, size_t offset)
{
   return unix_dkread(fd, buf, len, offset);
}

int kernel_write(void* buf, size_t len, size_t offset)
{
   return unix_dkwrite(fd, buf, len, offset);
}
