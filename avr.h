#ifndef _AVR_H
#define _AVR_H

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/io.h>

#define XTAL_FRQ 8000000lu

#define SET_BIT(p,i) ((p) |=  (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) &   (1 << (i)))

#define WDR() asm volatile("wdr"::)
#define NOP() asm volatile("nop"::)
#define RST() for(;;);

void avr_init(void);

void avr_wait(unsigned short msec);
int get_key();
int get_num();
struct datetime{
	short year;
	char month;
	char day;
	char hour;
	char minute;
	char second;
	char subsecond;
	char military;
};
void keep_time(struct datetime *);
void keep_date(struct datetime *);
int is_pressed(int, int);
void blink(int);
void set_time(struct datetime *);
void set_date(struct datetime *);
void display_time(struct datetime *);

#endif /* _AVR_H */