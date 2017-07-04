#include <msp430.h> 
#include <lcd.h>

/*
 * main.c
 */
int main(void) {

  WDTCTL = WDTPW + WDTHOLD;		//Desabilita Watchdog Timer Counter
  BCSCTL1 = CALBC1_1MHZ; 		// Set range
  DCOCTL  = CALDCO_1MHZ;  		// Set DCO step + modulation = 1MHz clock
  lcd_init();					//Função que inicializa LCD
  P1DIR = 0b01000001;           // Inicializando P1.0 e P1.6 como saída
  P1OUT = 0x00;                 // intialize bit 0 of P1 to 0
 
  while(1==1) {
      P1OUT = P1OUT ^ 0b00000001;   // Ativando a porta P1.0 (LED Vermelho)
      lcd_print("LED VERMELHO");
      lcd_cmd(LCD_FIRST_ROW);
      delay_s(1);                   // delay aprox 1s
      P1OUT = 0x00;                 // Apagando todos os leds    
      P1OUT = P1OUT ^ 0b01000000;   // Ativando a porta P1.6 (LED Verde)
      lcd_print("LED VERDE       ");
      delay_s(1);
      P1OUT = 0x00;                 // Apagando todos os leds    
      lcd_cmd(LCD_FIRST_ROW);     
  }
}

void delay_s(unsigned int ms) {
	while (ms) {
		__delay_cycles(1000000);    // 1s delay at 1MHz
		ms--;
	}
}
