#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <linux/types.h>

#include "krw.h"
#include "ebbchar.h"

unsigned long off4Text[] =
{
   0x146e0000, 0x00080000, 0x03102000, 0x0000000a,
   0x00000000, 0x00000000, 0x00000000, 0x644d5241,
   0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

unsigned long g_kernelSlide = 0;
unsigned long g_kernelBase = 0;

int openEbbchar(krwContext_t* fd)
{
   int result = -1;
   fd->fdIn = open("/dev/mychardev-0", O_RDONLY);
   fd->fdOut = open("/dev/mychardev-1", O_WRONLY);

   FAIL_IF(fd->fdIn < 0);
   FAIL_IF(fd->fdOut < 0);

   result = 0;
fail:
   return result;
}

int kSeek(krwContext_t* fd, size_t dst)
{
   size_t seekBuf[2] = {0};
   size_t setState = SEEK_OP;

   seekBuf[0] = setState;
   seekBuf[1] = dst;

   write(fd->fdOut, seekBuf, sizeof(seekBuf));
   return 0;
}

int kRead(krwContext_t* fd, void* buf, size_t len, size_t offset)
{
   void* newBuf = 0;

   kSeek(fd, offset);
   read(fd->fdIn, buf, len);
   return 0;
}

int kWrite(krwContext_t* fd, void* buf, size_t len, size_t offset)
{
   size_t setState = WRITE_OP;

   kSeek(fd, offset);
   
   memcpy(buf, &setState, sizeof(size_t));

   len += sizeof(size_t);
   write(fd->fdOut, buf, len);
   return 0;
}

int kSlide(krwContext_t* fd, size_t* a_kernelSlide)
{
   size_t printkAddr = 0;
   size_t kAddrIter = 0;
   size_t textBase = 0;
   int i = 0;
   size_t bufInterp[sizeof(off4Text) / sizeof(size_t)] = {0};
   int result = -1;

   kRead(fd, bufInterp, 8, (size_t)0);
   printkAddr = bufInterp[0];

   kAddrIter = printkAddr & PAGE_MASK;
   for (i = 0; i < 0x400; i++)
   {
      kRead(fd, bufInterp, sizeof(off4Text), kAddrIter);
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

int kBase(krwContext_t* fd, size_t* a_kernelBase)
{
   int result = -1;

   FINISH_IF(g_kernelBase != 0);

   FAIL_IF(kSlide(fd, NULL) == -1);

finish:
   if (a_kernelBase != 0)
   {
      *a_kernelBase = g_kernelBase;
   }
   result = 0;
fail:
   return result;
}

size_t kSlideTarg(krwContext_t* fd, size_t targ)
{
   size_t resultTarg = -1;
   FAIL_IF(kSlide(fd, NULL) == -1);

   resultTarg = targ + g_kernelSlide;
fail:
   return resultTarg;
}

size_t kBaseTarg(krwContext_t* fd, size_t targ)
{
   size_t resultTarg = -1;
   FAIL_IF(kSlide(fd, NULL) == -1);

   resultTarg = targ + g_kernelBase;
fail:
   return resultTarg;
}

template<typename unitLen>
void dumpMemT(unsigned char* base, size_t len, const char* format)
{
   size_t i = 0;
   printf("%04hx: ", (unsigned short)i);
   for (i = 0; i < len; i += (sizeof(unitLen)))
   {
      if (((i % 0x10) == 0) && (i != 0))
      {
         printf("\n");
         printf("UD:%04hx: ", (unsigned short)i);
      }
      printf(format, *((unitLen*)&base[i]));
   }
}

void dumpMem(unsigned char* base, size_t len, char format)
{
   printf("UD:");
   if (format == 's')
   {
      printf(" %s", (char*)base);
   }
   else
   {
      if (format == 'c')
      {
         dumpMemT<unsigned char>(base, len, "0x%02hhx ");
      }
      else if (format == 'h')
      {
         dumpMemT<unsigned short>(base, len, "0x%04hx ");
      }
      else if (format == 'w' || ((format == 0) && (sizeof(void*) == 4)))
      {
         dumpMemT<unsigned int>(base, len, "0x%08x ");
      }
      else if (format == 'q' || ((format == 0) && (sizeof(void*) == 8)))
      {
         dumpMemT<unsigned long>(base, len, "0x%016lx ");
      }
   }
   printf("\n");
}
