/**
 * @file   testebbchar.c
 * @author Derek Molloy
 * @date   7 April 2015
 * @version 0.1
 * @brief  A Linux user space program that communicates with the ebbchar.c LKM. It passes a
 * string to the LKM and reads the response from the LKM. For this example to work the device
 * must be called /dev/ebbchar.
 * @see http://www.derekmolloy.ie/ for a full description and follow-up descriptions.
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "ebbchar.h"
#include "krw.h"

int main(int argc, char** argv)
{
   int ret;
   SLIDE_BASE_t slid;
   opVals_t curOp;
   char format = 0;
   krwContext_t fd;
   void* addr = 0;
   size_t len = 0;
   unsigned char netBuf[KBUF_SIZE] = {0};
   unsigned char* readBuf = &netBuf[sizeof(size_t)];
   int i = 0;
   

   if (argc < 5)
   {
      printf("usage: %s op slide addr size", argv[0]);
      return -1;
   }

   if (*argv[1] == 'r')
   {
      curOp = READ_OP;
      format = argv[1][1];
   }
   else if (*argv[1] == 'w')
   {
      curOp = WRITE_OP;
   }
   else
   {
      printf("invalid op arg");
      return -1;
   }

   if (*argv[2] == 's')
   {
      slid = SLIDE_TARG;
   }   
   else if (*argv[2] == 'b')
   {
      slid = BASE_TARG;
   }
   else if (*argv[2] == 'u')
   {
      slid = UNTOUCHED_TARG;
   }
   else
   {
      printf("invalid slide arg");
      return -1;
   }
   
   addr = (void*)strtoull(argv[3], NULL, 0x10);
   len = strtoull(argv[4], NULL, 0x10);

   if ((len % sizeof(size_t)) != 0)
   {
      len += (sizeof(size_t) - (len % sizeof(size_t)));
   }

   openEbbchar(&fd);

   if (slid == SLIDE_TARG)
   {
      addr = (void*)kSlideTarg(&fd, (size_t)addr);
   }
   else if (slid == BASE_TARG)
   {
      addr = (void*)kBaseTarg(&fd, (size_t)addr);
   }

   FAIL_IF(len > KBUF_SIZE);
   if (curOp == READ_OP)
   {
      kRead(&fd, netBuf, len, (size_t)addr);
      dumpMem(netBuf, len, format);
   }
   else if (curOp == WRITE_OP)
   {
      kWrite(&fd, netBuf, len, (size_t)addr);
   }


fail:
   return 0;
}
