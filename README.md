# my-optee-projects
A collection of Trusted Applications (TA) and host applications developed while learning OP-TEE and GlobalPlatform APIs.

---

## 🏗️ Project Architecture

The workspace is organized into separate components bridging the Normal World (Linux) and the Secure World (OP-TEE OS):

* **`tee/`** : Host applications running in the Rich OS (Normal World) that interface with `libteec` to communicate with the TEE via `/dev/tee0`.
* **`ta_hmac/`** : A custom Trusted Application implementing secure cryptographic operations that never expose key material to the Normal World.
    * `ta_hmac.c` : Main TA logic utilizing GlobalPlatform Internal Core APIs.
    * `test_board/test_hmac_tee.py` : A Python validation script utilizing `ctypes` to dynamically interface with `libteec.so.1` directly on the target architecture.

---

## 🛠️ Environment Setup & Compilation

This project is tailored for the **STMicroelectronics STM32MP2** platform using the OpenSTLinux SDK (Yocto Scarthgap distribution).

### 1. Initialize the Cross-Compilation Environment
Before building any component, source the Yocto SDK environment to export the required architecture variables:

```
source /opt/st/stm32mp2/5.0.15-openstlinux-6.6-yocto-scarthgap-mpu-v26.02.18/environment-setup
```

### 2. Compile the HMAC Trusted Application (TA)
TAs run isolated inside the secure monitor and require a bare-metal toolchain (aarch64-none-elf-) rather than a standard Linux cross-compiler.

cd ta_hmac/

#### Define the path to the OP-TEE OS user developer kit provided by the SDK
export TA_DEV_KIT_DIR=$OECORE_TARGET_SYSROOT/usr/include/optee/export-user_ta_arm64

#### Define the bare-metal toolchain path
export BARE_METAL=$OECORE_NATIVE_SYSROOT/usr/share/gcc-aarch64-none-elf/bin/aarch64-none-elf-

#### Build and Sign the TA
make TA_DEV_KIT_DIR=$TA_DEV_KIT_DIR CROSS_COMPILE=$BARE_METAL
This compilation outputs a signed binary tracking the unique TA UUID:

UUID: 7d1e3d7b-abc5-4b77-8b31-abc3f5e2e432

Binary: 7d1e3d7b-abc5-4b77-8b31-abc3f5e2e432.ta

## 🚀 Deployment & Execution

Securely copy the signed cryptographic binary to the default location where the OP-TEE supplicant lookups for Trusted Applications (/usr/lib/optee_armtz/):

scp 7d1e3d7b-abc5-4b77-8b31-abc3f5e2e432.ta root@<TARGET_IP>:/usr/lib/optee_armtz/