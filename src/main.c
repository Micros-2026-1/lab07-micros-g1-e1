#pragma config FOSC = INTIO67   
#pragma config PLLCFG = OFF    
#pragma config PRICLKEN = ON    
#pragma config WDTEN = OFF      
#pragma config PWRTEN = OFF     
#pragma config BOREN = OFF      
#pragma config MCLRE = EXTMCLR  
#pragma config PBADEN = OFF    

#define _XTAL_FREQ 48000000UL
#include <xc.h>
#include "i2c.h"
#include "i2c_lcd.h"


void main()
{
 
    I2C_init();
    lcd_init();

    lcd_set_cursor(0, 0);
    lcd_write_string("Hola mundo PIC18F!");

    while (1)
    {
    }
}