#ifndef DRV_SHARE_H
#define DRV_SHARE_H

#define ANDROID_KERNBASE   0xFFFFFF8008080000

#define KBUF_SIZE 0x1000

#define CHARNAME "mario_drv"
#define CLASSDRV "mario"

typedef enum
{
    LEAK_PRINTK=0,
    LEAK_DEVREAD,
    LEAK_KERNMAX
} kern_leak_t;

#endif