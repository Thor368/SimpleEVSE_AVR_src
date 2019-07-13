/*
 * SimpleEVSE_AVR_src.c
 *
 * Created: 13.07.2019 17:25:14
 * Author : main
 */ 

#define F_CPU 9600000

#include <avr/io.h>
#include <util/delay.h>

#define LED_ON		PORTB |= 1 << PB3
#define LED_OFF		PORTB &= ~(1 << PB3)

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

int main(void)
{
	PORTB = 0;
	DDRB = 0b00001011;
	
	OCR0A = 149;
	OCR0B = 150;
	TCCR0A = 0b00110011;
	TCCR0B = 0b00001010;
	
	blink_LED_slow();
	
    while (1)
    {
    }
}

