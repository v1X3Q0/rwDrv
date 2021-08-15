#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "krw.h"
#include "ebbchar.h"

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
   opVals_t setState = SEEK_OP;

   kSeek(fd, offset);
   read(fd->fdIn, buf, len);
   return 0;
}

void dumpMem(unsigned char* base, size_t len)
{
   size_t i = 0;
   printf("UD: ");
   for (i = 0; i < len; i += (sizeof(unsigned long)))
   {
      if (((i % 0x10) == 0) && (i != 0))
      {
         printf("\n");
      }
      printf("0x%08lx ", *((unsigned long*)&base[i]));
   }
}
