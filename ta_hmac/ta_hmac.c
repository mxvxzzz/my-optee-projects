#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <string.h>
#include "ta_hmac.h"

#define KEY_ID      "stm32_hmac_key"
#define KEY_ID_LEN  14

TEE_Result TA_CreateEntryPoint(void)  { return TEE_SUCCESS; }
void       TA_DestroyEntryPoint(void) {}

TEE_Result TA_OpenSessionEntryPoint(uint32_t pt __unused,
                                     TEE_Param p[4] __unused,
                                     void **ctx __unused)
{
    return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void *ctx __unused) {}


/*
 * CMD 0 : generate key and store in secure storage.
 */
static TEE_Result cmd_gen_key(void)
{
    TEE_ObjectHandle obj = TEE_HANDLE_NULL;
    TEE_Result       res;
    uint8_t          key[TA_HMAC_KEY_SIZE];

    /* random bytes with RNG hardware */
    TEE_GenerateRandom(key, sizeof(key));

    /* Persist the key in this TA's private secure storage */
    res = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
                                     KEY_ID, KEY_ID_LEN,
                                     TEE_DATA_FLAG_ACCESS_WRITE |
                                     TEE_DATA_FLAG_OVERWRITE,
                                     TEE_HANDLE_NULL,
                                     key, sizeof(key),
                                     &obj);
    if (res != TEE_SUCCESS) {
        EMSG("CreatePersistentObject: 0x%x", res);
        goto out;
    }
    TEE_CloseObject(obj);
    IMSG("HMAC key generated and stored in OP-TEE secure storage");

out:
    TEE_MemFill(key, 0, sizeof(key)); /* delete ""RAM"" */
    return res;
}

/*
 * CMD 1: compute HMAC-SHA256(key, data).
 * params[0] MEMREF_INPUT  : data to authenticate
 * params[1] MEMREF_OUTPUT : HMAC tag (32 bytes)
 */
static TEE_Result cmd_compute(uint32_t pt, TEE_Param params[4])
{
    TEE_ObjectHandle  store   = TEE_HANDLE_NULL;
    TEE_ObjectHandle  key_obj = TEE_HANDLE_NULL;
    TEE_OperationHandle op    = TEE_HANDLE_NULL;
    TEE_Attribute     attr;
    TEE_Result        res;
    uint8_t           key[TA_HMAC_KEY_SIZE];
    size_t            key_len = sizeof(key);
    uint32_t          exp_pt;

    exp_pt = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                              TEE_PARAM_TYPE_MEMREF_OUTPUT,
                              TEE_PARAM_TYPE_NONE,
                              TEE_PARAM_TYPE_NONE);
    if (pt != exp_pt)
        return TEE_ERROR_BAD_PARAMETERS;

    if (params[1].memref.size < TA_HMAC_TAG_SIZE)
        return TEE_ERROR_SHORT_BUFFER;

    /* read the key from the TA private secure storage */
    res = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
                                   KEY_ID, KEY_ID_LEN,
                                   TEE_DATA_FLAG_ACCESS_READ,
                                   &store);
    if (res != TEE_SUCCESS) {
        EMSG("Key not found — call GEN_KEY first: 0x%x", res);
        return res;
    }
    res = TEE_ReadObjectData(store, key, sizeof(key), &key_len);
    TEE_CloseObject(store);
    if (res != TEE_SUCCESS)
        goto out;

    /* create the transient key object */
    res = TEE_AllocateTransientObject(TEE_TYPE_HMAC_SHA256, 256, &key_obj);
    if (res != TEE_SUCCESS) goto out;

    TEE_InitRefAttribute(&attr, TEE_ATTR_SECRET_VALUE, key, key_len);
    res = TEE_PopulateTransientObject(key_obj, &attr, 1);
    if (res != TEE_SUCCESS) goto out;

    res = TEE_AllocateOperation(&op, TEE_ALG_HMAC_SHA256, TEE_MODE_MAC, 256);
    if (res != TEE_SUCCESS) goto out;

    res = TEE_SetOperationKey(op, key_obj);
    if (res != TEE_SUCCESS) goto out;

    /* compute tag HMAC */
    TEE_MACInit(op, NULL, 0);
    res = TEE_MACComputeFinal(op,
                              params[0].memref.buffer,
                              params[0].memref.size,
                              params[1].memref.buffer,
                              &params[1].memref.size);

out:
    if (op      != TEE_HANDLE_NULL) TEE_FreeOperation(op);
    if (key_obj != TEE_HANDLE_NULL) TEE_FreeTransientObject(key_obj);
    TEE_MemFill(key, 0, sizeof(key));
    return res;
}

TEE_Result TA_InvokeCommandEntryPoint(void *ctx __unused,
                                       uint32_t cmd,
                                       uint32_t pt,
                                       TEE_Param params[4])
{
    switch (cmd) {
    case TA_HMAC_CMD_GEN_KEY: return cmd_gen_key();
    case TA_HMAC_CMD_COMPUTE: return cmd_compute(pt, params);
    default:                  return TEE_ERROR_BAD_PARAMETERS;
    }
}
