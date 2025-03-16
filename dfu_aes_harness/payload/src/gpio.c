#include "gpio.h"
#include "openiboot.h"

static GPIORegisters* GPIORegs;

int gpio_setup() {
	// iboot writes to these registers on the S5L8720
	// this probably is some weird replacement for a clock gate
	SET_REG(GPIOIC + 0x70, 0x32);
	SET_REG(GPIOIC + 0x6C, 3);
	while (!(GET_REG(GPIOIC + 0x7C) & 0x1));
	SET_REG(GPIOIC + 0x6C, 2);

	int i;

	GPIORegs = (GPIORegisters*) GPIO;

	for(i = 0; i < GPIO_NUMINTGROUPS; i++) {
		// writes to all the interrupt status register to acknowledge and discard any pending
		SET_REG(GPIOIC + GPIO_INTSTAT + (i * 0x4), GPIO_INTSTAT_RESET);

		// disable all interrupts
		SET_REG(GPIOIC + GPIO_INTEN + (i * 0x4), GPIO_INTEN_RESET);
	}

	return 0;
}

int gpio_pin_state(int port) {
	return ((GPIORegs[GET_BITS(port, 8, 5)].DAT & (1 << GET_BITS(port, 0, 3))) != 0);
}

void gpio_custom_io(int port, int bits) {
	SET_REG(GPIO + GPIO_FSEL, ((GET_BITS(port, 8, 5) & GPIO_FSEL_MAJMASK) << GPIO_FSEL_MAJSHIFT)
				| ((GET_BITS(port, 0, 3) & GPIO_FSEL_MINMASK) << GPIO_FSEL_MINSHIFT)
				| ((bits & GPIO_FSEL_UMASK) << GPIO_FSEL_USHIFT));
}

void gpio_pin_use_as_input(int port) {
	gpio_custom_io(port, 0);
}

void gpio_pin_output(int port, int bit) {
	gpio_custom_io(port, 0xE | bit); // 0b111U, where U is the argument
}
