#ifndef TA_HMAC_H
#define TA_HMAC_H

#define TA_HMAC_UUID { 0x7d1e3d7b, 0xabc5, 0x4b77, \
    { 0x8b, 0x31, 0xab, 0xc3, 0xf5, 0xe2, 0xe4, 0x32 } }

/*
 * Command 0 : TA_HMAC_CMD_GEN_KEY
 * Generates a random HMAC-SHA256 key and stores it
 * in the OP-TEE secure storage.
 * params: none
 */
#define TA_HMAC_CMD_GEN_KEY  0
/*
 * Command 1 : TA_HMAC_CMD_COMPUTE
 * Computes HMAC-SHA256(secret_key, data).
 * params[0]: MEMREF_INPUT : data to authenticate
 * params[1]: MEMREF_OUTPUT : resulting HMAC tag (32 bytes)
 */
#define TA_HMAC_CMD_COMPUTE  1

#define TA_HMAC_KEY_SIZE  32   /* key 256 bits */
#define TA_HMAC_TAG_SIZE  32   /* SHA-256 = 32 bytes */

#endif /* TA_HMAC_H */
