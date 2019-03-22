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
#define A 1784
#define NY 3
#define LN 7
#define BJ 15
#define NZ 20

void avr_init(void);

struct note{
	float freq;
	int dur;
};

void avr_wait(unsigned short msec);
void PlayNote(float freq, unsigned int duration);

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

struct time{
	char hour;
	char minute;
	char second;
	char subsecond;
};
void keep_time(struct datetime *);
void keep_date(struct datetime *);
void count_down(struct time *);
int check_alarm(struct datetime *, struct datetime[], int);
int is_pressed(int, int);
void blink(int);
void set_time(struct datetime *);
void set_timer(struct time *);
void set_date(struct datetime *);
void display_time(struct datetime *);
void display_timer(struct time *);
struct datetime world_calc(struct datetime *date, char offset);

#endif /* _AVR_H */