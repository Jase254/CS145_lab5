#include "avr.h"
#include <stdio.h>
#include <string.h>
#include "lcd.h"

void
avr_init(void)
{
	WDTCR = 15;
}

void 
PlayNote(float freq, unsigned int duration){
	float wav = (1/freq) * 1000;
	unsigned int cycles = duration/wav;
	float period = (wav / 2) * 100;
	
	while(cycles > 0){
		PORTA |= (1<<PA1);
		avr_wait(period);
		PORTA &= ~(1<<PA1);
		avr_wait(period);
		cycles--;
	}
}

void
avr_wait(unsigned short msec)
{
	TCCR0 = 3;
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.0001);
		SET_BIT(TIFR, TOV0);
		WDR();
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}
char str[17];
char out[17];


int main(void){
	// Initialize everything
	avr_init();	
	lcd_init();
	DDRA |= (1 << PA1);
	struct datetime dt = {2019, 3, 21, 11, 55, 0, 0, 0};
	struct time timer = {0, 0, 0, 0};
	display_time(&dt);
	struct datetime alarms[5];
	int num_alarms = 0;
	int sound_alarm = 0;
	int finished = 0;
	struct note n = {A, 30};
	for(;;){
		//Main loop will check if key pressed, and if it is A or B, do something
		avr_wait(850);
		keep_time(&dt);
		display_time(&dt);
		sound_alarm = check_alarm(&dt, alarms, num_alarms);
		
		if(sound_alarm){
			PlayNote(n.freq, n.dur);
		}
			 
		int key = get_key();
		switch(key){
			// Set date and time
			case 4:
				set_date(&dt);
				set_time(&dt);
				break;
			// Toggle military time
			case 8:
				dt.military = dt.military^1;
				avr_wait(1500);
				break;
			case 12:
				lcd_clr();
				lcd_pos(0,1);
				lcd_puts2("Set New Alarm");	
				avr_wait(10000);
				set_date(&alarms[num_alarms]);
				set_time(&alarms[num_alarms]);
				num_alarms++;
				break;
			case 16:
				lcd_clr();
				lcd_pos(0,1);
				lcd_puts2("Set New Timer");	
				avr_wait(10000);
				set_timer(&timer);
				finished = 0;
				while(!finished){
					avr_wait(850);
					count_down(&timer);
					display_timer(&timer);
					
					if((timer.hour <= 0) && 
					(timer.minute <= 0) && 
					(timer.second <= 0) &&
					(timer.subsecond <= 0)){
						finished = 1;
					}
				}
				PlayNote(n.freq, n.dur);
				break;
			default:
				break;
		}
	}
}

//Check alarms
int check_alarm(struct datetime *dt, struct datetime alarms[], int size){
	int alarm = 0;
	for( int i = 0; i < size; ++i){
		if((alarms[i].year == dt->year) &&
		(alarms[i].month == dt->month) &&
		(alarms[i].day == dt->day) &&
		(alarms[i].hour == dt->hour) &&
		(alarms[i].minute == dt->minute) && 
		(alarms[i].second == dt->second)){
			alarm = 1;
			break;
	}
	}
	return alarm;
}
void set_timer(struct time *tm){
	lcd_clr();
	lcd_pos(0,1);
	strcpy(str, "Hour: ");
	lcd_puts2(str);
	tm->hour = get_num();
	avr_wait(2000);
	
	lcd_clr();
	lcd_pos(0,1);
	strcpy(str, "Minute: ");
	lcd_puts2(str);
	tm->minute = get_num();
	avr_wait(2000);
	
	lcd_clr();
	lcd_pos(0,1);
	strcpy(str, "Second: ");
	lcd_puts2(str);
	tm->second = get_num();
	avr_wait(2000);
	tm->subsecond = 0;
}

void display_timer(struct time *tm){
	lcd_clr();
	lcd_pos(1,1);
	sprintf(out, "%02d:%02d:%02d:%d", tm->hour, tm->minute, tm->second, tm->subsecond);
	lcd_puts2(out);
}
	
// Display the time to lcd
void display_time(struct datetime *dt){
	sprintf(out, "%d/%d/%d", dt->month, dt->day, dt->year);
	lcd_clr();
	lcd_pos(0,1);
	lcd_puts2(out);
	lcd_pos(1,1);
	// If military time, display without special formatting (default)
	if(dt->military){
		sprintf(out, "%02d:%02d:%02d:%d", dt->hour, dt->minute, dt->second, dt->subsecond);
	}
	else{
		// Otherwise some special cases are in play for AM/PM
		if(dt->hour > 12){
			sprintf(out, "%02d:%02d:%02d:%d %s", (dt->hour - 12), dt->minute, dt->second, dt->subsecond, "PM");
		}
		else if(dt->hour == 0){
			sprintf(out, "%02d:%02d:%02d:%02d %s", (dt->hour + 12), dt->minute, dt->second, dt->subsecond, "AM");
		}
		else{
			sprintf(out, "%02d:%02d:%02d:%d %s", dt->hour, dt->minute, dt->second, dt->subsecond, "AM");
		}
	}
	lcd_puts2(out);
}

/************************************************************************/
/* Prompt user to enter in first minute, then hour.                     */
/************************************************************************/
void set_time(struct datetime *dt){
	lcd_clr();
	lcd_pos(0,1);
	strcpy(str, "Minute: ");
	lcd_puts2(str);
	dt->minute = get_num();
	avr_wait(2000);
	
	lcd_clr();
	lcd_pos(0,1);
	strcpy(str, "Hour: ");
	lcd_puts2(str);
	dt->hour = get_num();
	avr_wait(2000);
	dt->second = 0;
	dt->subsecond = 0;
}
/************************************************************************/
/* Prompt user to enter in first day, then month, then year             */
/************************************************************************/
void set_date(struct datetime *dt){
	lcd_clr();
	lcd_pos(0,1);
	strcpy(str, "Day: ");
	lcd_puts2(str);
	dt->day = get_num();
	avr_wait(2000);
	
	lcd_clr();
	lcd_pos(0,1);
	strcpy(str, "Month: ");
	lcd_puts2(str);
	dt->month = get_num();
	avr_wait(2000);
	
	lcd_clr();
	lcd_pos(0,1);
	strcpy(str, "Year: ");
	lcd_puts2(str);
	dt->year = get_num();
	avr_wait(2000);
}
/************************************************************************/
/* Gets actual keypad value (numbers 0-9)                               */
/************************************************************************/
int get_num(void){
	int num = 0;
	for(;;){
		int key = get_key();
		switch(key){
			// # is enter key
			case 15:
				return num;
			case 0:
			case 4:
			case 8:
			case 12: 
			case 13:
			case 16: // do nothing in this case
				break;
			default:
				key = key - ((key-1)/4);
				if(key == 11) key = 0;
				num = (num * 10) + key;
				sprintf(out, "%d", key);
				lcd_puts2(out);
				avr_wait(2000);
				break;
		}
	}
}

/************************************************************************/
/* Runs within main loop to keep time                                   */
/************************************************************************/
void keep_time(struct datetime *date){
	if(++(date->subsecond) > 9){
		date->subsecond = 0;
		if(++(date->second) > 59){
			date->second = 0;
			if(++(date->minute) > 59){
				date->minute = 0;
				if(++(date->hour) > 23){
					date->hour = 0;
					keep_date(date);
				}
			}
		}
	}
}

void count_down(struct time *tm){
	if(((tm->subsecond)-- == 0)){
		tm->subsecond = 9;
		if((tm->second)-- == 0){
			tm->second = 59;
			if((tm->minute)-- == 0){
				tm->minute = 59;
				if((tm->hour)-- == 0){
					tm->hour = 0;
				}
			}
		}
	}
}
/************************************************************************/
/* Called by keep time function                                         */
/************************************************************************/
void keep_date(struct datetime *date){
	date->day++;
	char extra = 0;
	switch(date->month){
		case 2:
			if((date->year % 4) == 0) extra = 1;
			if(date->day > (28+extra)){
				date->month++;
				date->day = 1;
			}
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			if(date->day > 30){
				date->month++;
				date->day = 1;
			}
			break;
		default:
			if(date->day > 31){
				date->day = 1;
				if(++(date->month)>12){
					date->month = 1;
					++(date->year);
				}
			}
			break;
	}
}

/************************************************************************/
/* Check for if a certain button is pressed                             */
/************************************************************************/
int is_pressed(int row, int col){
	//set all rows, cols to n/c
	DDRC=0;
	PORTC=0;
	//set col to strong 0
	SET_BIT(DDRC, col+4);
	//set row to weak 1
	SET_BIT(PORTC, row);
	avr_wait(1);
	return !GET_BIT(PINC, row);
}

/************************************************************************/
/* Get raw key pressed, different than get_num which does some conversion*/
/* for keypad numbers                                                   */
/************************************************************************/
int get_key(){
	int r,c;
	for(r=0;r<4;++r){
		for(c=0;c<4;++c){
			if(is_pressed(r,c)){
				return 1+(r*4)+c;
			}
		}
	}
	return 0;
}