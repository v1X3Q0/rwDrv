
#define FAIL_IF(x) \
    if (x) \
    { \
        printf("%s:%d\n", __FILE__, __LINE__); \
        goto fail; \
    }

#define SAFE_CLOSE(x) \
    if (x != 0) \
    { \
        close(x); \
        x = 0; \
    }

#define SAFE_FREE(x) \
    if (x != 0) \
    { \
        free(x); \
        x = 0; \
    }

#define SAFE_FCLOSE(x) \
    if (x != 0) \
    { \
        fclose(x); \
        x = 0; \
    }    

#define KBUF_SIZE 0x1000
#define KBUF_NET (KBUF_SIZE + sizeof(void*))

typedef enum opVals
{
    READ_OP=1,
    WRITE_OP=2,
    SEEK_OP=3
} opVals_t;

