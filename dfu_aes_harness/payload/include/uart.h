#pragma once

#include "openiboot.h"

// Devices
#define UART 0x3CC00000

// Registers
#define UART0 0x0
#define UART1 0x4000
#define UART2 0x8000
#define UART3 0xC000
#define UART4 0x10000

#define UART_ULCON 0x0
#define UART_UCON 0x4
#define UART_UFCON 0x8
#define UART_UMCON 0xC
#define UART_UTRSTAT 0x10
#define UART_UERSTAT 0x14
#define UART_UFSTAT 0x18
#define UART_UMSTAT 0x1C
#define UART_UTXH 0x20
#define UART_URXH 0x24
#define UART_UBAUD 0x28
#define UART_UDIVSLOT 0x2C

// Values
#define NUM_UARTS 5
#define UART_CLOCKGATE 0x29

#define UART_CLOCK_SELECTION_MASK (0x3 << 10) // Bit 10-11
#define UART_CLOCK_SELECTION_SHIFT 10 // Bit 10-11
#define UART_UCON_UNKMASK 0x7000
#define UART_UCON_LOOPBACKMODE (0x1 << 5)
#define UART_UCON_RXMODE_SHIFT 0
#define UART_UCON_RXMODE_MASK (0x3 << UART_UCON_RXMODE_SHIFT)
#define UART_UCON_TXMODE_SHIFT 2
#define UART_UCON_TXMODE_MASK (0x3 << UART_UCON_TXMODE_SHIFT)

#define UART_FIFO_RESET_TX 0x4
#define UART_FIFO_RESET_RX 0x2
#define UART_FIFO_ENABLE 0x1

#define UART_DIVVAL_MASK 0x0000FFFF
#define UART_SAMPLERATE_MASK 0x00030000 // Bit 16-17
#define UART_SAMPLERATE_SHIFT 16

#define UART_UCON_MODE_DISABLE 0
#define UART_UCON_MODE_IRQORPOLL 1
#define UART_UCON_MODE_DMA0 2
#define UART_UCON_MODE_DMA1 3

#define UART_CLOCK_PCLK 0
#define UART_CLOCK_EXT_UCLK0 1
#define UART_CLOCK_EXT_UCLK1 3

#define UART_SAMPLERATE_4 2
#define UART_SAMPLERATE_8 1
#define UART_SAMPLERATE_16 0

#define UART_UMCON_AFC_BIT 0x10
#define UART_UMCON_NRTS_BIT 0x1

#define UART_UTRSTAT_TRANSMITTEREMPTY 0x4
#define UART_UTRSTAT_RECEIVEDATAREADY 0x1

#define UART_UMSTAT_CTS 0x1

#define UART_UFSTAT_TXFIFO_FULL (0x1 << 9)
#define UART_UFSTAT_RXFIFO_FULL (0x1 << 8)
#define UART_UFSTAT_TXCOUNT_SHIFT 4
#define UART_UFSTAT_TXCOUNT_MASK (0xF << UART_UFSTAT_TXCOUNT_SHIFT)
#define UART_UFSTAT_RXCOUNT_MASK 0xF

#define UART_5BITS 0
#define UART_6BITS 1
#define UART_7BITS 2
#define UART_8BITS 3

#define UART_ULCON_UNKN 0x7

#define UART_POLL_MODE 0
#define UART_IRQ_MODE 1

typedef struct UARTSettings {
	uint32_t ureg;
	uint32_t baud;
	uint32_t sample_rate;
	OnOff flow_control;
	OnOff fifo;
	uint32_t mode;
	uint32_t clock;
} UARTSettings;

typedef struct UARTRegisters {
	uint32_t ULCON;
	uint32_t UCON;
	uint32_t UFCON;
	uint32_t UMCON;

	uint32_t UTRSTAT;
	uint32_t UERSTAT;
	uint32_t UFSTAT;
	uint32_t UMSTAT;

	uint32_t UTXH;
	uint32_t URXH;
	uint32_t UBAUD;
	uint32_t UDIVSLOT;
} UARTRegisters;

int uart_setup();
int uart_set_clk(int ureg, int clock);
int uart_set_sample_rate(int ureg, int rate);
int uart_set_flow_control(int ureg, OnOff flow_control);
int uart_set_mode(int ureg, uint32_t mode);
int uart_set_baud_rate(int ureg, uint32_t baud);
int uart_set_fifo(int ureg, OnOff fifo);
int uart_set(int ureg, uint32_t baud, uint32_t bits, uint32_t parity, uint32_t stop);
int uart_send_break_signal(int ureg, OnOff send);
int uart_set_rx_buf(int ureg, uint32_t mode, uint32_t size);


int uart_write(int ureg, const char *buffer, uint32_t length);
int uart_read(int ureg, char *buffer, uint32_t length, uint64_t timeout);