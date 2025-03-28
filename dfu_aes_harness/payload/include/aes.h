#pragma once

// Device
#define AES 0x38C00000

// Registers
#define CONTROL 0x0
#define GO 0x4
#define UNKREG0 0x8
#define STATUS 0xC
#define UNKREG1 0x10
#define KEYLEN 0x14
#define INSIZE 0x18
#define INADDR 0x20
#define OUTSIZE 0x24
#define OUTADDR 0x28
#define AUXSIZE 0x2C
#define AUXADDR 0x30
#define SIZE3 0x34
#define KEY 0x4C
#define TYPE 0x6C
#define IV 0x74

// Values

#define KEYSIZE 0x20
#define IVSIZE 0x10

#define AES_ENCRYPT 1
#define AES_DECRYPT 0

#define GET_KEYLEN(x) GET_BITS(x, 16, 2)

#define AES_CLOCKGATE 0xA


#define AES_128_CBC_IV_SIZE 16
#define AES_128_CBC_BLOCK_SIZE 64

typedef enum AESKeyType {
	AESCustom = 0,
	AESGID = 1,
	AESUID = 2
} AESKeyType;

typedef enum AESKeyLen {
	AES128 = 0,
	AES192 = 1,
	AES256 = 2
} AESKeyLen;


void aes_encrypt(void* data, int size, AESKeyType keyType, const void* key, AESKeyLen keyLen, const void* iv);
void aes_decrypt(void* data, int size, AESKeyType keyType, const void* key, AESKeyLen keyLen, const void* iv);