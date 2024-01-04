#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <linux/types.h>

#include <localUtil.h>

#include <krw_util.h>
#include <drv_share.h>

size_t g_kernelSlide = 0;
size_t g_kernelBase = 0;

int kernel_slide(size_t* a_kernelSlide)
{
   int result = -1;

   FINISH_IF(g_kernelSlide != 0);

   SAFE_BAIL(kernel_base(NULL) == -1);

finish:
   if (a_kernelSlide != 0)
   {
      *a_kernelSlide = g_kernelSlide;
   }
   result = 0;
fail:

   return result;
}   

int kernel_base(size_t* a_kernelBase)
{
   int result = -1;

   FINISH_IF(g_kernelBase != 0);

   SAFE_BAIL(kBaseRoll(a_kernelBase) == -1);

   g_kernelSlide = *a_kernelBase - ANDROID_KERNBASE;
   g_kernelBase = *a_kernelBase;
finish:
   if (a_kernelBase != 0)
   {
      *a_kernelBase = g_kernelBase;
   }
   result = 0;
fail:
   return result;
}

int kernel_close()
{
   return 0;
}

int kernel_read_ptr(void* buf, size_t len, size_t offset)
{
   return kernel_read(buf, len, offset);
}

int kernel_write_ptr(void* buf, size_t len, size_t offset)
{
   return kernel_write(buf, len, offset);
}
