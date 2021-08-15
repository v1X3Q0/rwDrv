#include <stdlib.h>

typedef struct krwContext
{
    int fdOut;
    int fdIn;
} krwContext_t;

int openEbbchar(krwContext_t* fd);
int meatyEbbchar(int fd);
int kSeek(krwContext_t* fd, size_t dst);
int kRead(krwContext_t* fd, void* buf, size_t len, size_t offset);
void dumpMem(unsigned char* base, size_t len);
