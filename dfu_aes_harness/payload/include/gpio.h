#pragma once

#include <stdint.h>

#define NOR_CS 0x406

// gpioicBase
// Device
#define GPIOIC 0x39700000	/* probably a part of the system controller */

// gpioBaseAddress
#define GPIO 0x3CF00000

// Registers
#define GPIO_INTLEVEL 0x80
#define GPIO_INTSTAT 0xA0
#define GPIO_INTEN 0xC0
#define GPIO_INTTYPE 0xE0
#define GPIO_FSEL 0x1E0

// Values
#define GPIO_NUMINTGROUPS 7
#define GPIO_INTSTAT_RESET 0xFFFFFFFF
#define GPIO_INTEN_RESET 0

#define GPIO_FSEL_MAJSHIFT 16
#define GPIO_FSEL_MAJMASK 0x1F
#define GPIO_FSEL_MINSHIFT 8
#define GPIO_FSEL_MINMASK 0x7
#define GPIO_FSEL_USHIFT 0
#define GPIO_FSEL_UMASK 0xF

typedef enum
{
	GPIOPDDisabled,
	GPIOPDUp,
	GPIOPDDown
} GPIOPDSetting;

typedef struct GPIORegisters {
	volatile uint32_t CON;
	volatile uint32_t DAT;
	volatile uint32_t PUD1;	// (PUD1[x], PUD2[x]): (0, 0) = pull up/down for x disabled, (0, 1) = pull down, (1, 0) = pull up
	volatile uint32_t PUD2;
	volatile uint32_t CONSLP1;
	volatile uint32_t CONSLP2;
	volatile uint32_t PUDSLP1;
	volatile uint32_t PUDSLP2;
} GPIORegisters;

/**
 *  Setup the GPIO interface and register
 *
 *  Returns 0 on success, OiB won't work if this fails.
 *
 *  @ingroup GPIO
 */
int gpio_setup();

void gpio_pin_output(int port, int bit);