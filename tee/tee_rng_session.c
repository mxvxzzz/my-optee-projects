/*
 * Open a session with a TA (trusted application) 
 * specially TA RNG 
 *
 *
 *
 $CC tee_rng_session.c -o tee_rng_session \
    -lteec \
    -I$OECORE_TARGET_SYSROOT/usr/include \
    -L$OECORE_TARGET_SYSROOT/usr/lib
*/
#include <stdio.h>
#include <string.h>
#include <tee_client_api.h>

/* UUID of TA RNG OP-TEE (in /sys/bus/tee/drivers/optee-rng) */
#define TA_RNG_UUID { 0xab7a617c, 0xb8e7, 0x4d8f, \
    { 0x83, 0x01, 0xd0, 0x9b, 0x61, 0x03, 0x6b, 0x64 } }

int main(void)
{
    TEEC_Context    ctx;
    TEEC_Session    sess;
    TEEC_UUID       uuid = TA_RNG_UUID;
    TEEC_Operation  op;
    TEEC_SharedMemory shm;
    TEEC_Result     res;
    uint32_t        origin;

    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) {
        printf(" InitializeContext: 0x%x\n", res);
        return 1;
    }

    res = TEEC_OpenSession(&ctx, &sess, &uuid,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (res != TEEC_SUCCESS) {
        printf("OpenSession: 0x%x\n", res);
        TEEC_FinalizeContext(&ctx);
        return 1;
    }
    printf("Session open with TA RNG\n");

    /* memory shared saved */
    shm.size  = 32;
    shm.flags = TEEC_MEM_OUTPUT;
    res = TEEC_AllocateSharedMemory(&ctx, &shm);
    if (res != TEEC_SUCCESS) {
        printf("AllocateSharedMemory: 0x%x\n", res);
        goto close;
    }
    printf("Shared memory allocate (%zu octets)\n", shm.size);

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(
        TEEC_MEMREF_WHOLE,
        TEEC_NONE,
        TEEC_NONE,
        TEEC_NONE);
    op.params[0].memref.parent = &shm;
    op.params[0].memref.offset = 0;
    op.params[0].memref.size   = shm.size;

    res = TEEC_InvokeCommand(&sess, 0, &op, &origin);
    if (res == TEEC_SUCCESS) {
        printf("%zu octets by OP-TEE RNG : ", shm.size);
        for (size_t i = 0; i < shm.size; i++)
            printf("%02x", ((unsigned char *)shm.buffer)[i]);
        printf("\n");
    } else {
        printf("InvokeCommand: 0x%x (origin=0x%x)\n", res, origin);
    }

    TEEC_ReleaseSharedMemory(&shm);

close:
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    return 0;
}