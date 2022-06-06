#define F_CPU 16000000   

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>                                 
#include <util/delay.h>	
#include <avr/interrupt.h>

#define LCD_Dir  DDRD                                                           
#define LCD_Port PORTD                                                          
#define RS PD2                                                                  
#define EN PD3                                                                  

int notas, count_s, pot, count;
int posicion = 0;
int vel_cancion = 0;
int duracion = 30;
int flag_s = 0;
int flag = 1;
int tiempo = 0;
int start = 0;
int row = 0;
int pos = 0;
int points = 0;

char score[3];
unsigned char asteroid[]= {0x00,0x0E,0x1F,0x1F,0x1F,0x0E,0x00,0x00};
unsigned char starship[]= {0x0E,0X04,0X0E,0X15,0X0E,0X04,0X0E,0X00};
int asteroid_l0[] = {4, 6, 12, 18, 25, 26, 31, 32, 37, 38};
int asteroid_l1[] = {65, 73, 74, 79, 85, 87, 92, 93, 98, 99};

int starwarstheme[]  = {0,34,0,34,0,34,25,17,19,20,22,12,17,19,20,22,12,17,19,20,19,22,0,
                        0,34,0,34,0,34,25,17,19,20,22,12,17,19,20,22,12,17,19,20,19,22,0,
                        0,34,0,34,0,34,25,17,19,20,22,12,17,19,20,22,12,17,19,20,19,22,0,
                        0,34,0,34,0,34,25,17,19,20,22,12,17,19,20,22,12,17,19,20,19,22,0,
                        0,34,0,34,0,34,25,17,19,20,22,12,17,19,20};

int t_lento[]  = {32, 16, 16, 4, 32, 104, 104, 48, 32, 32, 24, 16, 2, 4, 16, 4, 4, 2, 4, 4, 8, 4, 2, 24, 48, 32, 32, 
                 24, 16, 2, 4, 16, 4, 4, 2, 4, 4, 8, 4, 2, 64, 0, 0, 0, 8, 4, 0, 0, 8, 2, 2, 24, 0, 0, 0, 8, 4, 8, 8,
                 104, 0, 0, 0, 8, 4, 0, 0, 8, 2, 2, 24, 48, 48, 208, 0, 0, 0, 8, 4, 0, 0, 8, 2, 2, 24, 0, 0, 0, 8, 4,
                 8, 8, 104, 0, 0, 0, 8, 4, 0, 0, 8, 2, 2, 24, 48, 48, 208};

int t_normal[]  = {16, 8, 8, 2, 16, 52, 52, 24, 16, 16, 12, 8, 1, 2, 8, 2, 2, 1, 2, 2, 4, 2, 1, 12, 24, 16, 16, 12, 8,
                  1, 2, 8, 2, 2, 1, 2, 2, 4, 2, 1, 32, 0, 0, 0, 4, 2, 0, 0, 4, 1, 1, 12, 0, 0, 0, 4, 2, 4, 4, 52, 0, 0,
                  0, 4, 2, 0, 0, 4, 1, 1, 12, 24, 24, 104, 0, 0, 0, 4, 2, 0, 0, 4, 1, 1, 12, 0, 0, 0, 4, 2, 4, 4, 52, 0,
                  0, 0, 4, 2, 0, 0, 4, 1, 1, 12, 24, 24, 104};

int t_rapido[]  = {8, 4, 4, 1, 8, 26, 26, 12, 8, 8, 6, 4, 0, 1, 1, 1, 1, 0, 1, 1, 2, 1, 0, 6, 12, 8, 8, 6, 4, 0, 1, 1, 1,
                  1, 0, 1, 1, 2, 1, 0, 16, 0, 0, 0, 2, 1, 0, 0, 2, 0, 0, 6, 0, 0, 0, 2, 1, 2, 2, 26, 0, 0, 0, 2, 1, 0, 0,
                  2, 0, 0, 6, 12, 12, 52, 0, 0, 0, 2, 1, 0, 0, 2, 0, 0, 6, 0, 0, 0, 2, 1, 2, 2, 26, 0, 0, 0, 2, 1, 0, 0, 
                  2, 0, 0, 6, 12, 12, 52};

void LCD_Command(unsigned char cmnd);
void LCD_Char(unsigned char data);
void CreateCustomCharacter(unsigned char *Pattern, const char Location);
void LCD_String_xy(char row, char pos);
void LCD_String(char *str);
void LCD_Clear();
void Game_Init();

ISR(TIMER0_OVF_vect) {
    TCNT0 = 96;                                                                 
    TIFR0 = 1<<TOV0;
    if (start == 1) {
        if (tiempo < 188) { 
            ++count_s;
            if (count_s == notas && flag_s == 0 && tiempo < 188) {
                PORTB = 0x02;                                                   
                count_s = 0;         
                flag_s = 1;
            } 
            if (count_s == notas && flag_s == 1 && tiempo < 188) {
                PORTB = 0x00;                                                   
                count_s = 0;         
                flag_s = 0;
            }
            ++count;
            if (count == 30000 && flag == 0 && tiempo < 30) { 
                if (pos < 39) ++pos;
                else pos = 0;
                LCD_String_xy(row, pos);
                LCD_Char(1);
            
                ++tiempo;
                for (int i = 0; i < 10; i++) {
                    if (pos == asteroid_l0[i] && row == 0) ++points;
                    if ((pos + 64) == asteroid_l1[i] && row == 1) ++points;
                }
                LCD_Command(0x18);
            
                vel_cancion = 0;
                count = 0;       
                flag = 1;
            }
            if (count == 30000 && flag == 1 && tiempo < 30) {  
                int i = 0;
                int coll = 0;
                while (i < 10 && coll == 0) {
                    if (pos == asteroid_l0[i] && row == 0) {
                        LCD_String_xy(row, pos);
                        LCD_Char(2);
                        coll = 1;
                    }
                
                    if ((pos + 64) == asteroid_l1[i] && row == 1) {
                        LCD_String_xy(row, pos);
                        LCD_Char(2);
                        coll = 1;
                    }
                    ++i;
                }
                
                if (coll == 0) {
                    LCD_String_xy(row, pos);
                    LCD_String(" ");
                }
            
                vel_cancion = 0;
                count = 0;         
                flag = 0;
            }
            if (count == 15000 && flag == 0 && (tiempo >= 30 && tiempo < 90)) {
                if (pos < 39) ++pos;
                else pos = 0;
                LCD_String_xy(row, pos);
                LCD_Char(1);
            
                ++tiempo;
                for (int i = 0; i < 10; i++) {
                    if (pos == asteroid_l0[i] && row == 0) points += 2;
                    if ((pos + 64) == asteroid_l1[i] && row == 1) points += 2;
                }
                LCD_Command(0x18);
            
                vel_cancion = 1;
                count = 0;       
                flag = 1;
            }
            if (count == 15000 && flag == 1 && (tiempo >= 30 && tiempo < 90)) {
                int i = 0;
                int coll = 0;
                while (i < 10 && coll == 0) {
                    if (pos == asteroid_l0[i] && row == 0) {
                        LCD_String_xy(row, pos);
                        LCD_Char(2);
                        coll = 1;
                    }
                
                    if ((pos + 64) == asteroid_l1[i] && row == 1) {
                        LCD_String_xy(row, pos);
                        LCD_Char(2);
                        coll = 1;
                    }
                    ++i;
                }
            
                if (coll == 0) {
                    LCD_String_xy(row, pos);
                    LCD_String(" ");
                }
            
                vel_cancion = 1;
                count = 0;         
                flag = 0;
            }
            if (count == 7500 && flag == 0 && (tiempo >= 90 && tiempo < 188)) {
                if (pos < 39) ++pos;
                else pos = 0;
                LCD_String_xy(row, pos);
                LCD_Char(1);
            
                ++tiempo;
                for (int i = 0; i < 10; i++) {
                    if (pos == asteroid_l0[i] && row == 0) points += 3;
                    if ((pos + 64) == asteroid_l1[i] && row == 1) points += 3;
                }
                LCD_Command(0x18);
                
                vel_cancion = 2;
                count = 0;       
                flag = 1;
            }
            if (count == 7500 && flag == 1 && (tiempo >= 90 && tiempo < 188)) {
                int i = 0;
                int coll = 0;
                while (i < 10 && coll == 0) {
                    if (pos == asteroid_l0[i] && row == 0) {
                        LCD_String_xy(row, pos);
                        LCD_Char(2);
                        coll = 1;
                    }
                
                    if ((pos + 64) == asteroid_l1[i] && row == 1) {
                        LCD_String_xy(row, pos);
                        LCD_Char(2);
                        coll = 1;
                    }
                    ++i;
                }
            
                if (coll == 0) {
                    LCD_String_xy(row, pos);
                    LCD_String(" ");
                }
            
                vel_cancion = 2;
                count = 0;         
                flag = 0;
            }
        } else {                                                                
            start = 0;
            
            _delay_ms(500);
            LCD_Clear();
            itoa(180 - points, score, 10);                                      
            LCD_String("Puntos:");
            LCD_String_xy(0, 8);
            LCD_String(score);
            
            _delay_ms(3000);
            LCD_Clear();

            posicion = 0;
            vel_cancion = 0;
            
            flag_s = 0;
            count_s = 0;
            
            tiempo = 0;
            flag = 1;
            count = 0;
            points = 0;
            pos = 0;
            row = 0;
            
            Game_Init();
            _delay_ms(250);
            
            start = 1;
        }
    }
}

void ADC_Init() {
	DDRC = 0x0;                                                                 
	ADCSRA = 0x87;                                                              
	ADMUX = 0x40;                                                               
}

int ADC_Read(char channel) {
	int Ain, AinLow;

	ADMUX = ADMUX|(channel & 0x0f);                                             

	ADCSRA |= (1<<ADSC);                                                        
	while((ADCSRA&(1<<ADIF)) == 0);                                             

	_delay_us(10);
	AinLow = (int)ADCL;                                                         
	Ain = (int)ADCH*256;                                                        
	Ain = Ain + AinLow;
	return(Ain);                                                                
}

void LCD_Command(unsigned char cmnd) {
	LCD_Port = (LCD_Port & 0x0F) | (cmnd & 0xF0);                               
	LCD_Port &= ~ (1<<RS);                                                      
	LCD_Port |= (1<<EN);                                                        
	_delay_us(10);
	LCD_Port &= ~ (1<<EN);

	_delay_us(200);

	LCD_Port = (LCD_Port & 0x0F) | (cmnd << 4);                                 
	LCD_Port |= (1<<EN);
	_delay_us(10);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(20);
}

void LCD_Char(unsigned char data) {
	LCD_Port = (LCD_Port & 0x0F) | (data & 0xF0);                               
	LCD_Port |= (1<<RS);                                                        
	LCD_Port |= (1<<EN);
	_delay_us(10);
	LCD_Port &= ~ (1<<EN);

	_delay_us(200);

	LCD_Port = (LCD_Port & 0x0F) | (data << 4);                                 
	LCD_Port |= (1<<EN);
	_delay_us(10);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(20);
}

void CreateCustomCharacter(unsigned char *Pattern, const char Location) {
    int i = 0;
    LCD_Command(0x40+(Location*8));                                             
    for(i = 0; i < 8; i++)
        LCD_Char(Pattern[i]);                                                   
}

void LCD_Init(void) {                                                            
	LCD_Dir = 0xFF;                                                             
	_delay_ms(20);                                                               
	CreateCustomCharacter(starship, 1);
    CreateCustomCharacter(asteroid, 2);
	LCD_Command(0x02);                                                          
	LCD_Command(0x28);                                                          
	LCD_Command(0x0C);                                                          
	LCD_Command(0x06);                                                          
	LCD_Command(0x01);                                                          
	_delay_ms(2);
}

void LCD_String(char *str) {                                                    
	int i;
	for(i = 0; str[i] != 0; i++)                                                
		LCD_Char(str[i]);
}

void LCD_String_xy(char row, char pos) {                           
	if (row == 0 && pos<40)
        LCD_Command((pos & 0x3f)|0x80);                                         
	else if (row == 1 && pos<104)
        LCD_Command((pos & 0x3f)|0xC0);                                                                                                   
}

void LCD_Clear() {
	LCD_Command(0x01);                                                          
	_delay_ms(2);
	LCD_Command(0x80);                                                          
}

void duration(unsigned char milliseconds) {
    while(milliseconds > 0) {
        milliseconds--;
        _delay_ms(1);
    }
}

void rests(int i) {
    PORTB = 0x00;                                                                
    count_s = 0;         
    flag_s = 1;
    duration(duracion);   
}

void Game_Init() {
    LCD_String("Tec de Monterrey");                                             
	LCD_Command(0xC0);                                                          
	LCD_String("****Equipo 2****");                                             
    _delay_ms(1500);
    LCD_Clear();
    LCD_String("   Star Wars   ");                                             
	LCD_Command(0xC0);                                                          
	LCD_String("*****Galaga*****");                                             
    _delay_ms(1500);
    LCD_Clear();
    
    // asteroids
    for(unsigned char i = 0; i < 10; i++) {
        // fila superior del LCD
        LCD_String_xy(0, asteroid_l0[i]);
        LCD_Char(2);
        // fila inferior del LCD
        LCD_String_xy(1, asteroid_l1[i]);
        LCD_Char(2);
    }
        
    //starship
    LCD_String_xy(0, 0);
    LCD_Char(1);
    _delay_ms(250);
    LCD_String_xy(0, 0);
    LCD_String(" ");
    LCD_String_xy(0, 1);
    LCD_Char(1);
    ++pos;
    
    LCD_Command(0x0C);
}

int main() {
    DDRB = 0xFF;                                                                // Salidas (B5, B1)
    TCCR0B = 0x01;
    TCNT0 = 0x00;
    TIFR0 = 1<<TOV0;
    TIMSK0 = 1<<TOIE0;
    sei();
    
	ADC_Init();
	LCD_Init();                                                                 
    Game_Init();
    _delay_ms(250);
    start = 1;                                                                  
    
	while(1) {
        if(posicion < 107) {
            if (vel_cancion == 0) {                                                   
                duracion = 30;
                notas = starwarstheme[posicion];
                duration(t_lento[posicion]);
                rests(posicion);
                ++posicion;
            } else if (vel_cancion == 1) {                                            
                duracion = 20;
                notas = starwarstheme[posicion];
                duration(t_normal[posicion]);
                rests(posicion);
                ++posicion;
            } else {                                                            
                duracion = 10;
                notas = starwarstheme[posicion];
                duration(t_rapido[posicion]);
                rests(posicion);
                ++posicion;
            }
        } else posicion = 0;
        start = 0;
        pot = ADC_Read(0);
        if (pot > 511) { 
            if (row != 1) {
                row = 1;
                LCD_String_xy(0, pos);
                LCD_String(" ");
            }
        } else {
            if (row != 0) {
                row = 0;
                LCD_String_xy(1, pos);
                LCD_String(" ");
            }
        }
        start = 1;
	}
}
