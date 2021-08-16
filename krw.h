#include <stdlib.h>

#define ANDROID_KERNBASE   0xFFFFFF8008080000
// #define PAGE_SIZE 0x1000
#define PAGE_MASK (~(PAGE_SIZE - 1))

typedef enum SLIDE_BASE
{
    SLIDE_TARG,
    BASE_TARG,
    UNTOUCHED_TARG,
} SLIDE_BASE_t;

typedef struct krwContext
{
    int fdOut;
    int fdIn;
} krwContext_t;

int openEbbchar(krwContext_t* fd);
int meatyEbbchar(int fd);
int kSeek(krwContext_t* fd, size_t dst);
int kRead(krwContext_t* fd, void* buf, size_t len, size_t offset);
int kWrite(krwContext_t* fd, void* buf, size_t len, size_t offset);
int kSlide(krwContext_t* fd, size_t* a_kernelSlide);
int kBase(krwContext_t* fd, size_t* a_kernelBase);
size_t kSlideTarg(krwContext_t* fd, size_t targ);
size_t kBaseTarg(krwContext_t* fd, size_t targ);

void dumpMem(unsigned char* base, size_t len, char format);
