#ifndef USER_TA_HEADER_DEFINES_H
#define USER_TA_HEADER_DEFINES_H

#define TA_UUID { 0x7d1e3d7b, 0xabc5, 0x4b77, \
    { 0x8b, 0x31, 0xab, 0xc3, 0xf5, 0xe2, 0xe4, 0x32 } }

#define TA_FLAGS        (TA_FLAG_SINGLE_INSTANCE | TA_FLAG_MULTI_SESSION)
#define TA_STACK_SIZE   (2 * 1024)
#define TA_DATA_SIZE    (32 * 1024)
#define TA_VERSION      "1.0"
#define TA_DESCRIPTION  "STM32 HMAC TA | Key in OP-TEE"

#endif
