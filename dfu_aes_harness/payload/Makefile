CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
LD := arm-none-eabi-ld
PY3 := python3

CFLAGS := -march=armv6 -mthumb -nostdlib -I./include/

# if this breaks and i have to write a linker script i will cry
LDFLAGS := $(CFLAGS) -lgcc -static -Wl,-z,max-page-size=0x1000 -Wl,-Ttext-segment=0x22001000

OBJS := crt0.o main.o gpio.o uart.o clock.o aes.o
TARGET := payload.elf
TARGET_BIN := payload.bin

# TODO: do something smarter
INCLUDES = $(wildcard include/*.h)

.PHONY: clean

all: $(TARGET_BIN)

%.o: src/%.c $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@

crt0.o: src/crt0.S
	$(AS) -c $^ -o $@

$(TARGET): $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

%.bin: %.elf
	$(PY3) elf2bin.py $< $@

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET_BIN)
