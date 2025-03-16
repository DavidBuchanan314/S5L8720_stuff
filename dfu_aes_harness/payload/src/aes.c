#include <stddef.h>

#include "aes.h"
#include "clock.h"
#include "gpio.h"

static void initVector(const void* iv);

static void loadKey(const void *key);

static void doAES(int operation, void *buffer0, void *buffer1, void *buffer2, int size0, AESKeyType keyType, const void *key, int option0, int option1, int size1, int size2, int size3);

__attribute__((target("arm")))
static void CleanAndInvalidateCPUDataCache(void)
{
    asm volatile("mov r0,#0x0 \n"\
    "mcr p15,0x0,r0,cr7,cr14,0x0  \n"\
    "mcr p15,0x0,r0,cr7,cr10,0x4");
}


void aes_encrypt(void* data, int size, AESKeyType keyType, const void* key, AESKeyLen keylen, const void* iv) {
	clock_gate_switch(AES_CLOCKGATE, ON);
	SET_REG(AES + CONTROL, 1);
	SET_REG(AES + UNKREG1, 0);

	CleanAndInvalidateCPUDataCache();

	initVector(iv);

	CleanAndInvalidateCPUDataCache();

	// call AES internal function
	doAES(AES_ENCRYPT, data, data, data, size, keyType, key, 0, 1, size, size, size);

	//memset((void*)(AES + KEY), 0, KEYSIZE);
	//memset((void*)(AES + IV), 0, IVSIZE);


}

void aes_decrypt(void* data, int size, AESKeyType keyType, const void* key, AESKeyLen keylen, const void* iv) {
	clock_gate_switch(AES_CLOCKGATE, ON);
	SET_REG(AES + CONTROL, 1);
	SET_REG(AES + UNKREG1, 0);

	CleanAndInvalidateCPUDataCache();

	initVector(iv);

	CleanAndInvalidateCPUDataCache();

	// call AES internal function
	doAES(AES_DECRYPT, data, data, data, size, keyType, key, 0, 1, size, size, size);

	//memset((void*)(AES + KEY), 0, KEYSIZE);
	//memset((void*)(AES + IV), 0, IVSIZE);
}


static void initVector(const void* iv) {
	int i;
	uint32_t* ivWords = (uint32_t*) iv;
	uint32_t* ivRegs = (uint32_t*) (AES + IV);
	if(iv == NULL) {
		for(i = 0; i < (AES_128_CBC_IV_SIZE / 4); i++) {
			ivRegs[i] = 0;
		}
	} else {
		for(i = 0; i < (AES_128_CBC_IV_SIZE / 4); i++) {
			ivRegs[i] =	(ivWords[i] & 0x000000ff) << 24
					| (ivWords[i] & 0x0000ff00) << 8
					| (ivWords[i] & 0x00ff0000) >> 8
					| (ivWords[i] & 0xff000000) >> 24;
		}
	}
}

static void loadKey(const void *key) {
	AESKeyType keyType = GET_REG(AES + TYPE);

	if(keyType != AESCustom)
		return;			// hardware will handle key

	AESKeyLen keyLen = GET_KEYLEN(GET_REG(AES + KEYLEN));

	uint32_t keyAddr;
	int keyWords;
	switch(keyLen) {
		case AES256:
			keyAddr = AES + KEY + 256/8 - 256/8;
			keyWords = 256/8/4;
			break;
		case AES192:
			keyAddr = AES + KEY + 256/8 - 192/8;
			keyWords = 192/8/4;
			break;
		case AES128:
			keyAddr = AES + KEY + 256/8 - 128/8;
			keyWords = 128/8/4;
			break;
		default:
			return;
	}

	uint32_t* keyRegs = (uint32_t*) keyAddr;
	uint32_t* aKeyWords = (uint32_t*) key;
	int i;
	for(i = 0; i < keyWords; i++) {
		keyRegs[i] =	(aKeyWords[i] & 0x000000ff) << 24
				| (aKeyWords[i] & 0x0000ff00) << 8
				| (aKeyWords[i] & 0x00ff0000) >> 8
				| (aKeyWords[i] & 0xff000000) >> 24;
	}

}

static void doAES(int operation, void *buffer0, void *buffer1, void *buffer2, int size0, AESKeyType keyType, const void *key, int option0, int option1, int size1, int size2, int size3) {
	
	SET_REG(AES + UNKREG0, 1);
	SET_REG(AES + UNKREG0, 0);
	SET_REG(AES + CONTROL, 1);
	SET_REG(AES + TYPE, keyType);

	loadKey(key);

	SET_REG(AES + KEYLEN, (option1 << 3) | (option0 << 4) | 6);						// set bits 1 and 2
	SET_REG(AES + INSIZE, size0);
	SET_REG(AES + INADDR, (uint32_t) buffer0);
	SET_REG(AES + OUTSIZE, size2);
	SET_REG(AES + OUTADDR, (uint32_t) buffer1);
	SET_REG(AES + AUXSIZE, size1);
	SET_REG(AES + AUXADDR, (uint32_t) buffer2);
	SET_REG(AES + SIZE3, size3);

    gpio_pin_output(NOR_CS, 0); // indicate to the outside world that we're starting
	SET_REG(AES + GO, 1);

    // wait for it to be done
    while((GET_REG(AES + STATUS) & 0xF) == 0);

    gpio_pin_output(NOR_CS, 1); // tell the outside world
}