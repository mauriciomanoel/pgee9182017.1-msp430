/*lcd.h
* include this header file in your projects
* 16x2 LCD is used
* 
* P2.0 - D4 - 11
* P2.1 - D5 - 12
* P2.2 - D6 - 13
* P2.3 - D7 - 14
* P2.4 - RS - 4
* P2.5 - E  - 6
* GND  - R/W - 5
* GND  - VEE - 3
* TP1  - VCC - 2
*/

#include <msp430.h>
 
#define LCD_BIT_4 0x28 //4 bit mode(2 lines and 5×7 matrix)
#define LCD_BIT_8 0x38 //8 bit mode(2 lines and 5×7 matrix)
 
#define LCD_FIRST_ROW 0x80 //first row
#define LCD_SECOND_ROW 0xC0 //2nd row
 
#define LCD_DISPLAY_ON_CURSOR_ON 0x0F //Display On Cursor Blink
#define LCD_DISPLAY_OFF 0x08 //DISPLAY OFF CURSOR OFF -TURN OFF LCD
#define LCD_DISPLAY_ON_CURSOR_OFF 0x0C //DISPLAY ON CURSOR OFF
#define LCD_UNDERLINE_ON 0x0E //UNDERLINE ON
 
#define LCD_CLEAR 0x01 //Clear Display
#define LCD_RETURN_HOME 0x02 //RETURN HOME

#define LCD_JUMP_LINE 0xC0 // JUMP LINE
#define LCD_ADVANCE_COLUMN 0xC1 // Advance a column on the LCD 
 
#define LCD_INCREMENT_CURSOR 0x06 //Increment Cursor
#define LCD_DECREENT_CURSOR 0x04 //decrement Cursor
 
#define LCD_MOVE_CURSOR_LEFT 0x10 //MOVE CURSOR POS TO LEFT
#define LCD_MOVE_CURSOR_RIGHT 0x14 //MOVE CURSOR POS TO RIGHT
 
#define LCD_SHIFT_LEFT 0x18 //SHIFT ENTIRE DISPLAY TO LEFT
#define LCD_SHIFT_RIGHT 0x1C //SHIFT ENTIRE DISPLAY TO LEFT
  
#define DATA P2OUT = P2OUT | BIT4 // define RS high
#define COMMAND P2OUT = P2OUT & (~BIT4) // define RS low
 
#define ENABLE_HIGH P2OUT = P2OUT | BIT5 // define Enable high signal
#define ENABLE_LOW P2OUT = P2OUT & (~BIT5) // define Enable Low signal
unsigned int i;
unsigned int j;
 
void delay(unsigned int k) {
    for(j=0;j<=k;j++) {
        for(i=0;i<100;i++);
    }
}
 
void trigger_write(void) {
    ENABLE_HIGH;
    delay(2);
    ENABLE_LOW;
}
 
void lcd_cmd(unsigned char cmd) {
    COMMAND;
    P2OUT = (P2OUT & 0xF0)|((cmd>>4) & 0x0F); // transfer higher nibble
    trigger_write(); // trigger enable 
    P2OUT = (P2OUT & 0xF0)|(cmd & 0x0F); // write lower nibble
    trigger_write(); // trigger enable 
}
 
 
void lcd_chr(unsigned char data) {
    DATA;
    P2OUT = (P2OUT & 0xF0)|((data>>4) & 0x0F); // transfer higher nibble
    trigger_write(); // enable
    P2OUT = (P2OUT & 0xF0)|(data & 0x0F); // transfer lower nibble
    trigger_write(); // enable
}
 
void lcd_print(char *str) {
    while(*str) {
        lcd_chr(*str);
        str++;
    }
}
 
void lcd_init(void) {
    P2DIR |= 0xFF;   //Port2 as output
    P2OUT &= 0x00;   //port2 intial value as 0
    lcd_cmd(0x33);
    lcd_cmd(0x32);
    lcd_cmd(LCD_BIT_4);
    lcd_cmd(0x0E); //display on cursor blink
    lcd_cmd(LCD_CLEAR);
    lcd_cmd(LCD_INCREMENT_CURSOR);
    lcd_cmd(LCD_FIRST_ROW);
}