/*
 * File:   main.c
 * Author: L03038590
 *
 * Created on 14 de marzo de 2022, 06:54 PM
 */

#include <stdio.h>
#include <stdlib.h>

#define F_CPU 16000000UL			/* Define CPU Frequency e.g. here 8MHz */
#include <avr/io.h>			/* Include AVR std. library file */
#include <util/delay.h>			/* Include Delay header file */

#define LCD_Dir  DDRD			/* Define LCD data port direction */
#define LCD_Port PORTD			/* Define LCD data port */
#define RS PD2				/* Define Register Select pin */
#define EN PD3 				/* Define Enable signal pin */
unsigned char Pattern1[]= {0x0e,0x0e,0x04,0x04,0x1f,0x04,0x0a,0x0a};

void ADC_Init()
{
	DDRC=0x0;			/* Make ADC port as input ATmega328p pag 206*/
	ADCSRA = 0x87;			/* Enable ADC, fr/128  ATmega328p pag 218*/
	ADMUX = 0x40;			/* Vref: Avcc, ADC channel: 0 ATmega328p pag 217*/

}

int ADC_Read(char channel)
{
	int Ain,AinLow;

	ADMUX=ADMUX|(channel & 0x0f);	/* Set input channel to read */

	ADCSRA |= (1<<ADSC);		/* Start conversion */
	while((ADCSRA&(1<<ADIF))==0);	/* Monitor end of conversion interrupt */

	_delay_us(10);
	AinLow = (int)ADCL;		/* Read lower byte ATmega328p pag 219*/
	Ain = (int)ADCH*256;		/* Read higher 2 bits and
					Multiply with weight */
	Ain = Ain + AinLow;
	return(Ain);			/* Return digital value*/
}
/* LCD.pdf pag 21*/
void LCD_Command( unsigned char cmnd )
{
	LCD_Port = (LCD_Port & 0x0F) | (cmnd & 0xF0); /* sending upper nibble */
	LCD_Port &= ~ (1<<RS);		/* RS=0, command reg. */
	LCD_Port |= (1<<EN);		/* Enable pulse */
	_delay_us(10);
	LCD_Port &= ~ (1<<EN);

	_delay_us(200);

	LCD_Port = (LCD_Port & 0x0F) | (cmnd << 4);  /* sending lower nibble */
	LCD_Port |= (1<<EN);
	_delay_us(10);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(20);
}

void CreateCustomCharacter (unsigned char *Pattern, const char Location)
{
int i=0;
LCD_Command (0x40+(Location*8));     //Send the Address of CGRAM
for (i=0; i<8; i++)
LCD_Char (Pattern [ i ] );         //Pass the bytes of pattern on LCD
}

void LCD_Char( unsigned char data )
{
	LCD_Port = (LCD_Port & 0x0F) | (data & 0xF0); /* sending upper nibble */
	LCD_Port |= (1<<RS);		/* RS=1, data reg. */
	LCD_Port|= (1<<EN);
	_delay_us(10);
	LCD_Port &= ~ (1<<EN);

	_delay_us(200);

	LCD_Port = (LCD_Port & 0x0F) | (data << 4); /* sending lower nibble */
	LCD_Port |= (1<<EN);
	_delay_us(10);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(20);
}

void LCD_Init (void)			/* LCD Initialize function */
{
	LCD_Dir = 0xFF;			/* Make LCD port direction as o/p */
	_delay_ms(20);			/* LCD Power ON delay always >15ms */
	CreateCustomCharacter(Pattern1,1);
	LCD_Command(0x02);		/* send for 4 bit initialization of LCD  */
	LCD_Command(0x28);              /* 2 line, 5*7 matrix in 4-bit mode */
	LCD_Command(0x0c);              /* Display on cursor off*/
	LCD_Command(0x06);              /* Increment cursor (shift cursor to right)*/
	LCD_Command(0x01);              /* Clear display screen*/
	_delay_ms(2);
}


void LCD_String (char *str)		/* Send string to LCD function */
{
	int i;
	for(i=0;str[i]!=0;i++)		/* Send each char of string till the NULL */
	{
		LCD_Char (str[i]);
	}
}

void LCD_String_xy (char row, char pos, char *str)	/* Send string to LCD with xy position */
{
	if (row == 0 && pos<16)
	LCD_Command((pos & 0x0F)|0x80);	/* Command of first row and required position<16 */
	else if (row == 1 && pos<16)
	LCD_Command((pos & 0x0F)|0xC0);	/* Command of first row and required position<16 */
	LCD_String(str);		/* Call LCD string function */
}

void LCD_Clear()
{
	LCD_Command (0x01);		/* Clear display */
	_delay_ms(2);
	LCD_Command (0x80);		/* Cursor at home position */
}

int main()
{

	LCD_Init();			/* Initialization of LCD*/

	LCD_String("TEC de Monterrey");	/* Write string on 1st line of LCD*/
	LCD_Command(0xC0);		/* Go to 2nd line*/
	LCD_String("**Puebla**");	/* Write string on 2nd line*/
    _delay_ms(2000);
    LCD_Clear();
    LCD_Command ( 0x86 ) ;    //Place cursor at 6th position of first row
    LCD_Char(1) ;
     _delay_ms(2000);
    LCD_Clear();
    char String[5];
	int value;

	ADC_Init();
	LCD_Init();			/* Initialization of LCD */
	LCD_String("ADC value");	/* Write string on 1st line of LCD */

	while(1)
	{

		LCD_Command(0xc4);	/* LCD16x2 cursor position */
		value=ADC_Read(0);	/* Read ADC channel 0 */
		itoa(value,String,10);	/* Integer to string conversion */
		LCD_String(String);
		LCD_String("  ");
	}
}
