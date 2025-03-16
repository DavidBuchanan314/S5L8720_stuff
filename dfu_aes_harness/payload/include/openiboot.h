#pragma once

#include <stdint.h>

#define ERROR_BIT			(0x80000000)
#define ERROR(x)			((x) | ERROR_BIT)
#define ERROR_CODE(x)		((x) &~ ERROR_BIT)
#define FAILED(x)			(((x) & ERROR_BIT) != 0)
#define SUCCEEDED(x)		(((x) & ERROR_BIT) == 0)
#define SUCCESS_VALUE(x)	((x) &~ ERROR_BIT)

#define FAIL_ON(x, y)			do { if(x) { return y; } } while(0)
#define SUCCEED_ON(x)			FAIL_ON((x), SUCCESS)
#define CHAIN_FAIL(x)			do { error_t __val = (x); if(FAILED(__val)) { return __val; } } while(0)
#define CHAIN_FAIL_STORE(x, y)	do { if(SUCCEEDED(x)) { x = (y); } } while(0)

/**
 *  The error type enumeration.
 *
 *  This is the different kinds of errors OiB can return upon execution
 *  of a function.
 *
 *  @ingroup Error
 */
enum
{
	SUCCESS = 0,                /* Success */
	EINVAL = ERROR(1),          /* Invalid argument */
	EIO = ERROR(2),             /* I/O Error */
	ENOENT = ERROR(3),          /* No such file */
	ENOMEM = ERROR(4),          /* Out of Memory */
	ETIMEDOUT = ERROR(5),       /* Timed out */
};

typedef uint32_t error_t;

typedef enum OnOff {
	OFF = 0,
	ON = 1
} OnOff;

typedef volatile uint32_t reg32_t;
typedef volatile uint16_t reg16_t;
typedef volatile uint8_t reg8_t;

#define GET_REGX(s, x)		(*((volatile reg##s##_t*)(x)))
#define SET_REGX(s, x, y)	(*((volatile reg##s##_t*)(x)) = (y))

#define GET_REG8(x)			GET_REGX(8, (x))
#define SET_REG8(x, y)		SET_REGX(8, (x), (y))

#define GET_REG16(x)		GET_REGX(16, (x))
#define SET_REG16(x, y)		SET_REGX(16, (x), (y))

#define GET_REG32(x)		GET_REGX(32, (x))
#define SET_REG32(x, y)		SET_REGX(32, (x), (y))

#define GET_REG				GET_REG32
#define SET_REG				SET_REG32

#define GET_BITS(x, start, length) ((((uint32_t)(x)) << (32 - ((start) + (length)))) >> (32 - (length)))