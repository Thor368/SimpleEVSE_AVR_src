/*
 * SimpleEVSE_AVR_src.c
 *
 * Created: 13.07.2019 17:25:14
 * Author : main
 */ 

#define F_CPU 9600000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#define LED_ON			PORTB |= 1 << PB3
#define LED_OFF			PORTB &= ~(1 << PB3)
#define RELAIS_ON		PORTB |= 1 << PB0
#define RELAIS_OFF		PORTB &= ~(1 << PB0)
#define HALL			!(PINB & 0b100)
#define SET_SPEED(x)	OCR0B = ((uint16_t) x)*25/10 - 1

uint8_t eep_fresh_EE EEMEM;
uint8_t charge_speed_EE EEMEM;

void blink_LED_slow()
{
	for (uint8_t i = 0; i < 5; i++)
	{
		LED_ON;
		_delay_ms(500);
		LED_OFF;
		_delay_ms(500);
	}
}

void blink_LED_fast()
{
	for (uint8_t i = 0; i < 20; i++)
	{
		LED_ON;
		_delay_ms(125);
		LED_OFF;
		_delay_ms(125);
	}
}

void blink_LED_full()
{
	LED_ON;
	_delay_ms(5000);
	LED_OFF;
}

void show_speed(uint8_t speed)
{
	if (speed <= 6)
		blink_LED_slow();
	else if (speed <= 12)
		blink_LED_fast();
	else
		blink_LED_full();
}

uint16_t ADC_read()
{
	ADCSRA |= 1 << ADSC;
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

uint8_t hall_read()
{
	uint8_t cc = 0;
	while (HALL && (cc < 100))
	{
		cc++;
		_delay_ms(1);
	}
	
	if (cc >= 100)
		return 1;
	else
		return 0;
}

int main(void)
{
	PORTB = 0b100;
	DDRB = 0b00001011;
	
	OCR0A = 149;
	OCR0B = 150;
	TCCR0A = 0b00110011;
	TCCR0B = 0b00001011;
	
	ADMUX = 0b10;
	ADCSRA = 0b11000101;
	
	if (eeprom_read_byte(&eep_fresh_EE))
	{
		eeprom_write_byte(&eep_fresh_EE, 0);
		eeprom_write_byte(&charge_speed_EE, 6);
	}
	
	uint8_t charge_speed = eeprom_read_byte(&charge_speed_EE);
	show_speed(charge_speed);
	
    while (hall_read())
    {
		if (charge_speed == 6)
			charge_speed = 12;
		else if (charge_speed == 12)
			charge_speed = 16;
		else if (charge_speed == 16)
			charge_speed = 6;
		else
			charge_speed = 6;
		
		show_speed(charge_speed);
    }
	
	eeprom_update_byte(&charge_speed_EE, charge_speed);
	
	uint16_t fb = ADC_read();
	while(fb > 580)
		fb = ADC_read();
	
	SET_SPEED(charge_speed);
	RELAIS_ON;
	
	while(1);
}
