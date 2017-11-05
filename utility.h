#pragma once

#define saveAndExit(errorcode) {                \
        saveResult();                           \
        exit(errorcode);                        \
    }

#ifdef __DEBUG__
#define DEBUG_PRINT(fmt, args...)                                       \
    fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##args)
#else /* Don't do anything in non-debug builds */
#define DEBUG_PRINT(fmt, args...)
#endif
