
extern int PCF;
#include <wiringPi.h>


#define Adresa 0x27
#define PCF 100

#define RS PCF+0
#define RW PCF+1
#define E PCF+2
#define LED PCF+3
#define D4 PCF+4
#define D5 PCF+5
#define D6 PCF+6
#define D7 PCF+7


int handle;

void lcd_strobe(void);

void lcd_write(unsigned char c ,int mode)
{
	unsigned char d,b,i;
	d=c;
	d= (d>>4);
	for(i= 0; i<4;i++)
	{
		b= d&1;
		digitalWrite(PCF+4+i,b);
		d= d >>1;
	}
	digitalWrite(RS,mode);
	lcd_strobe();
	
	d=c;
	for(i=0;i<4;i++)
	{
		b = d&1;
		digitalWrite(PCF+4+i,b);
		d= d>>1;
	}
	digitalWrite(RS,mode);
	lcd_strobe();
	delay(0.1);
	digitalWrite(RS,HIGH);
}


// strobe function

void lcd_strobe(void)
{
	digitalWrite(E,HIGH);
	delay(0.1);
	digitalWrite(E,LOW);
	delay(0.1);
}


//function clear lcd 

void lcd_clear(void)
{
	lcd_write(0x01,0);
	delay(5);
}
void lcd_home(void)
{
	lcd_write(0x02,0);
	delay(5);
}
void lcd_cursor_blink_on()
{
	lcd_write(0x0D,0);
	delay(1);
}

void lcd_cursor_blink_off()
{
	lcd_write(0x0C,0);
	delay(1);
}

void lcd_cursors_off()
{
	lcd_write(0x0C,0);
	delay(1);
}

void lcd_cursors_on()
{
	lcd_write(0x0E,0);
	delay(1);
}

void lcd_puts(const char *s)
{
	while(*s) lcd_write(*s++,1);
}

void lcd_putch(unsigned char c )
{
	lcd_write(c,1);
}

void lcd_goto(int col,int row)
{
	char address ,c;
	c=col +1 ;
	if(row == 0) address =0;
	if(row ==1)address = 0x40;
	address += c -1;
	lcd_write(0x80 | address,0);
}



void lcd_init(void)
{
	char i;
	delay(120);
	for(i=0 ; i<8 ;i++)digitalWrite(PCF+i,0);
	delay(50);
	digitalWrite(D4,1);digitalWrite(D5,1);
	lcd_strobe();
	delay(10);
	lcd_strobe();
	delay(10);
	lcd_strobe();
	delay(10);
	digitalWrite(D4,0);
	lcd_strobe();
	delay(5);
	lcd_write(0x28,0);
	delay(1);
	lcd_write(0x08,0);
	delay(1);
	lcd_write(0x01,1);
	delay(10);
	lcd_write(0x06,0);
	delay(5);
	lcd_write(0x0C,0);
	delay(10);
	digitalWrite(LED,1);
}
