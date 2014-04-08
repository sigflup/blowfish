#include <libmega.h>

#define Z80_HALT_PORT		0xA11100
#define Z80_RESET_PORT		0xA11200
#define Z80_RAM			0xA00000
#define Z80_BANK_REGISTER	0xA06000

#define YM2612_BASEPORT		0xA04000

extern int bank_fm, bank_psg, bank_dac;

void Z80_init()
{
    Z80_requestBus(1);
    Z80_setBank(0);
}

u16 Z80_isBusTaken() {
 volatile u16 *pw;
 pw = (u16 *)Z80_HALT_PORT;
 if(*pw & 0x0100) return 0;
 else return 1;
}

void Z80_requestBus(u16 wait)
{
    volatile u16 *pw_bus;
    volatile u16 *pw_reset;

    // request bus (need to end reset)
    pw_bus = (u16 *) Z80_HALT_PORT;
    pw_reset = (u16 *) Z80_RESET_PORT;

    // bus not yet taken ?
    if (*pw_bus & 0x100)
    {
        *pw_bus = 0x0100;
        *pw_reset = 0x0100;

        if (wait)
        {
            // wait for bus taken
            while (*pw_bus & 0x0100);
        }
    }
}

void Z80_releaseBus()
{
    volatile u16 *pw;

    pw = (u16 *) Z80_HALT_PORT;
    *pw = 0x0000;
}


void Z80_startReset()
{
    volatile u16 *pw;

    pw = (u16 *) Z80_RESET_PORT;
    *pw = 0x0000;
}

void Z80_endReset()
{
    volatile u16 *pw;

    pw = (u16 *) Z80_RESET_PORT;
    *pw = 0x0100;
}


void Z80_setBank(const u16 bank)
{
    volatile u8 *pb;
    u16 i, value;

    pb = (u8 *) Z80_BANK_REGISTER;

    i = 9;
    value = bank;
    while (i--)
    {
        *pb = value;
        value >>= 1;
    }
}


void Z80_upload(const u16 to, const u8 *from, const u16 size, const u16 resetz80)
{
    Z80_requestBus(1);

    // copy data to Z80 RAM (need to use byte copy here)
    u8* src = (u8*) from;
    u8* dst = (u8*) (Z80_RAM + to);
    u16 len = size;

    while(len--) *dst++ = *src++;

    if (resetz80) Z80_startReset();
    Z80_releaseBus();
    // wait bus released
    while(Z80_isBusTaken());
    if (resetz80) Z80_endReset();
}

void YM2612_writeSafe(const u16 port, const u8 data) {
 volatile u8 *pb;
 pb = (u8 *)YM2612_BASEPORT;
 while(*pb&0x80);
 pb[port&3] = data;
}

void YM2612_reset() {
    u16 i;

    Z80_requestBus(1);

    YM2612_writeSafe(0,0);
    YM2612_writeSafe(2,0);
    for(i=0;i<0xff;i++) {
     YM2612_writeSafe(1,0x00);
     YM2612_writeSafe(3,0x00);
    } 

    bank_fm = 0;
    bank_psg = 0;
    bank_dac = 0;
    /*
    for(i = 0; i < 3; i++)
    {
        YM2612_writeSafe(0, 0xB4 | i);
        YM2612_writeSafe(1, 0xC0);
        YM2612_writeSafe(2, 0xB4 | i);
        YM2612_writeSafe(3, 0xC0);
    }

    YM2612_writeSafe(0, 0x22);
    YM2612_writeSafe(1, 0x00);

    YM2612_writeSafe(0, 0x27);
    YM2612_writeSafe(1, 0x00);

    YM2612_writeSafe(0, 0x28);
    for (i = 0; i < 3; i++)
    {
        YM2612_writeSafe(1, 0x00 | i);
        YM2612_writeSafe(1, 0x04 | i);
    }

    YM2612_writeSafe(0, 0x2B);
    YM2612_writeSafe(1, 0x00);
*/

    Z80_releaseBus();

}
