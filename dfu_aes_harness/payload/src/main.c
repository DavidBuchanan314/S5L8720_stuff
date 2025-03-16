#include <stdint.h>
#include <stddef.h>

#include "uart.h"
#include "gpio.h"
#include "clock.h"
#include "aes.h"

uint32_t (*aes_crypto_cmd)(uint32_t action, void *dst, void *src,size_t length,uint32_t keytype, void *key, void *iv) = (void*)0x899;
uint32_t (*do_hw_aes_decrypt)(void *buf, size_t length, uint32_t type, void *key, void *iv) = (void*)0x4f4d;

uint32_t ctr = 0;
uint8_t gold[0x10] = {216, 236, 125, 66, 179, 238, 249, 60, 175, 195, 250, 99, 140, 105, 226, 158};

#pragma align(0x40)
uint8_t aes_buf[0x10];

uint32_t woot[4];

// 8 blocks takes 48uS
// 6 blocks takes 46uS
// 0x100 blocks takes 220uS

int main()
{
    volatile uint32_t *magic = (void*)0x22026380;
    //__clear_cache(&magic, &magic+1);
    //clock_setup(); // if we comment this out, it makes the AES like, twice as fast
    gpio_setup();
    gpio_pin_output(NOR_CS, 0);
    //uart_setup();
    while (1) {
        for (int i=0; i<8; i++) {
            magic[i] = 0xdeadbeef;
        }
    }

    while (1) {
        for (int i=0; i<sizeof(aes_buf); i+=4) {
            *(uint32_t*)&aes_buf[i] = 0;
        }
        //aes_crypto_cmd(0x11, aes_buf, NULL, 0x10, 0x200, NULL, NULL);

        gpio_pin_output(NOR_CS, 1);
        do_hw_aes_decrypt(aes_buf, 0x10, 1, NULL, NULL);
        //aes_crypto_cmd(0x11, aes_buf, aes_buf, 0x10, 512, NULL, NULL);
        gpio_pin_output(NOR_CS, 0);

        /*uart_write(0, "hi0\n", 4);
        uart_write(1, "hi1\n", 4); // hangs (bc flow control is on?)
        uart_write(2, "hi2\n", 4); // hangs (bc flow control is on?)
        uart_write(3, "hi3\n", 4); // hangs (bc flow control is on?)
        uart_write(4, "hi4\n", 4);*/
        
        /*for (int i=0; i<sizeof(aes_buf); i+=0x10) {
            uint32_t *word = (uint32_t*)&aes_buf[i];
            int faulted = 0;
            for (int j=0; j<4; j++) {
                if (word[j] && (word[j] != gold4[j])) {
                    faulted = 1;
                    break;
                }
            }
            if (faulted) {
                for (int j=0; j<4; j++) {
                    woot[j] = word[j];
                }
                break;
            }
        }
        for (int j=0; j<4; j++) {
            magic2[j] = woot[j];
        }*/
        *magic = ctr++;
    }
    return 0;
}
