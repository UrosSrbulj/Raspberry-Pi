
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wiringPi.h>
#include "lcdinc.h"
#include <pcf8574.h>
#include <wiringPiI2C.h>
#include <math.h>


#define MAX_TEMP_LEN 256
#define W1_DEVICE "/sys/bus/w1/devices/28-3c10f6488967/w1_slave"
#define PWM_PIN 18  
#define adresa 0x27
#define PCF 100
#define PCF8591 (0x48) 
#define RELEY_PIN 21

float temperature = 0;
int fd, adcVrednost;
float voltage, vlaznostZemljista;

int senzor_temperature(char *filename ,float *temp)
{
	int fd,read_bytes; 
	char buff[MAX_TEMP_LEN];
	char temp_str[6];
	char *temp_ptr;
	
	if((fd = open(filename,O_RDONLY)) < 0)
	{
		perror("Ne mogu otvoriti W1 uredjaj");
		return -1;
		
	}
	
	if((read_bytes= read(fd,buff,MAX_TEMP_LEN)) <0)
	{
		perror("Greska u citanju");
		close(fd);
		return -1;
	}
	
	if((temp_ptr=strstr(buff,"t=")) ==NULL)
	{
		printf("Niije moguce pronaci temperaturu u fajli\n");
		close(fd);
		return -1;
	}
	
	strncpy(temp_str,temp_ptr +2,5);
	temp_str[5] = '\0';
	*temp = roundf(strtof(temp_str,NULL) / 100)/10;
	
	close(fd);
	return 0;
}

void kontrola_brzine(int temperature)
{
    int pwmVrednost;
    
    if (temperature <= 26) 
    {
		
        pwmVrednost = 0;
        lcd_goto(0,1);
        lcd_puts("Ventilator off");
    } 
    else if(temperature > 26) 
    {
        pwmVrednost = 1024;
        lcd_goto(0,1);
        lcd_puts("Ventilator on");  
    } 
    
    pwmWrite(PWM_PIN, pwmVrednost);
}

void termoregulacija(void) 
{
    float temperature;
    if (senzor_temperature(W1_DEVICE, &temperature) == 0) 
    {
        printf("Temperatura: %.1fC\n", temperature);
        lcd_clear(); 
        lcd_goto(0, 0);
        char buffer[32]; 
        sprintf(buffer, "Temperatura: %.1fC", temperature); 
        lcd_puts(buffer); 

        kontrola_brzine(temperature); 
    } else 
    {
        printf("Greska pri citanju temperature\n");
    }
    
    delay(3); 
}


void kontrola_navodnjavanja(int fd) 
{	
  int adcVrednost= wiringPiI2CReadReg8(fd, PCF8591 + 0x04);
  float voltage = (float)adcVrednost / 255 * 3.3;
  float vlaznostZemljista = (voltage / 3.3) * 100; 

  printf("ADC vrednost: %d,Napon: %.1fV,Vlaznost: %.1f%%\n",adcVrednost,voltage,vlaznostZemljista);
  lcd_clear(); 
  lcd_goto(0, 0); 
  char buffer[32]; 
  sprintf(buffer, "Vlaznost: %.1f%%", vlaznostZemljista);
  lcd_puts(buffer); 
  lcd_goto(0,1);

  if (vlaznostZemljista < 50) 
  {
      digitalWrite(RELEY_PIN, HIGH);
      printf("Relej ukljucen\n");
      lcd_puts("Pumpa ukljucena");
    } else 
    {
      digitalWrite(RELEY_PIN, LOW);
      printf("Relej iskljucen\n");
      lcd_puts("Pumpa iskljucena");
    }

    delay(3); 
}

	
int main(void)
	
{	
		
	pcf8574Setup(PCF,adresa);
	wiringPiSetupGpio();
	pinMode(PWM_PIN,PWM_OUTPUT);
	pinMode(RELEY_PIN, OUTPUT);
	lcd_init();
	
	if((fd = wiringPiI2CSetup(PCF8591)) < 0) 
	{
        printf("greska pri inicijalizaciji I2C\n");
        return 1;
	}
  
	while(1)
	{
	  termoregulacija();
	  kontrola_navodnjavanja(fd);	
	}
	
  return 0;
}
