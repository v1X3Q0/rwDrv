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

#define BUFFER_LENGTH 256           ///< The buffer length (crude but fine)
static char receive[BUFFER_LENGTH]; ///< The receive buffer from the LKM


int meatyEbbchar(int fd)
{
   int ret = 0;
   char stringToSend[BUFFER_LENGTH];

   printf("Type in a short string to send to the kernel module:\n");
   scanf("%[^\n]%*c", stringToSend); // Read in a string (with spaces)
   printf("Writing message to the device [%s].\n", stringToSend);
   ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
   if (ret < 0)
   {
      perror("Failed to write the message to the device.");
      return errno;
   }

   printf("Press ENTER to read back from the device...\n");
   getchar();

   printf("Reading from the device...\n");
   ret = read(fd, receive, BUFFER_LENGTH); // Read the response from the LKM
   if (ret < 0)
   {
      perror("Failed to read the message from the device.");
      return errno;
   }

   return 0;
}

int main(int argc, char** argv)
{
   int ret;
   krwContext_t fd;
   void* printkAddr = 0;
   void* addr = 0;
   size_t len = 0;
   unsigned char readBuf[KBUF_SIZE] = {0};

   if (argc < 3)
   {
      printf("usage: %s addr size", argv[0]);
      return -1;
   }

   addr = (void*)strtoull(argv[1], NULL, 0x10);
   len = strtoull(argv[2], NULL, 0x10);

   // printf("using 0x%08lx with len 0x%08lx\n", (size_t)addr, (size_t)len);
   // printf("Starting device test code example...\n");
   openEbbchar(&fd);

   // meatyEbbchar(fd);
   if (len <= KBUF_SIZE)
   {
      kRead(&fd, readBuf, len, (size_t)addr);
   }

   dumpMem(readBuf, len);

   printf("The received message is: [%s]\n", receive);
   printf("End of the program\n");
   return 0;
}
