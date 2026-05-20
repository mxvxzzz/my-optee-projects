// $CC -Wall -O2 hello.c -o hello_tee -lteec

// test communication with OP-TEE via libteec
#include <stdio.h>
#include <tee_client_api.h>

int main(void)
{
    TEEC_Context ctx;
    TEEC_Result  res;

    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed: 0x%x\n", res);
        return 1;
    }

    printf("TEE context open, communication OP-TEE KO(/dev/tee0)\n");
    TEEC_FinalizeContext(&ctx);
    return 0;
}