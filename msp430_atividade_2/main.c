#include <msp430.h>
#include <lcd.h>

#define BUTTON1 BIT3
#define BUTTON2 BIT2
#define LED_VERDE BIT6
#define LED_VERMELHO BIT0
#define FLIP_HOLD (0x3300 | WDTHOLD)    // flip HOLD while preserving  other bits
#define S1_MAKE P1OUT |= LED_VERMELHO   // turn on LED1 for demo
#define S1_BREAK P1OUT &= ~LED_VERMELHO // turn off LED1 for demo

// Define variable global
unsigned int tempo;
char value[5];
char* concatenate_string(char* str1, char* str2);

/*
 * main.c
 */
int main(void) {
  
  WDTCTL = WDTPW | WDTHOLD | WDTNMIES | WDTNMI; // Watchdog Timer Counter - Will be used exclusively to debounce s1 
  IFG1 &= ~(WDTIFG | NMIIFG); 
  IE1 |= WDTIE | NMIIE; 
  
  BCSCTL1 = CALBC1_1MHZ; 		// Set range
  DCOCTL  = CALDCO_1MHZ;  		// Set DCO step + modulation = 1MHz clock
  
  P1DIR = 0b01000011; // Ativando BIT6 e BIT0 como saida
  
  P1REN = BUTTON1;    // enables pull-up/ pull-down resistor of P1.3.
  P1OUT = BUTTON1;    // enable P1.3 in low
  P1IE  = BUTTON1;	  // Habilitando a interrupção p/ P1.3
  P1IES = BUTTON1;	  // Definição da borda subida/descida
  P1IFG = 0x00;	      // Registrador referente a flag
 
  lcd_init();         //Função que inicializa LCD
  tempo = 6;          // Valor inicial
  
  int_to_char(tempo,value,10); // Convertendo de inteiro para char
  lcd_cmd(LCD_FIRST_ROW);
  lcd_print(value);	
  lcd_print("s ");
  
  _enable_interrupts();	// Habilita as interrupções para o butão P1.3
  	
  while(1) {
        acende_led_verde();
        delay_s(tempo);
    }
}

void acende_led_verde() {
    P1OUT ^= LED_VERDE;
    __delay_cycles(500000);
    P1OUT ^= LED_VERDE;		// Troca o estado do led
}

void delay_s(unsigned int ms) {
	while (ms) {
		__delay_cycles(1000000); // 1s delay at 1MHz
		ms--;
	}
}

// Definição do vetor de interrupção
#pragma vector = PORT1_VECTOR	
__interrupt void Port_1(void) {
    // Debouncing 
    P1IES ^= BUTTON1;
	P1OUT ^= LED_VERMELHO;
	__delay_cycles(250000);
	
	P1IFG &= ~BUTTON1;
	// Reseta quando chegar no máximo		
	if (tempo >= 60) {
	    tempo = 0;
	}
	
	tempo = tempo + 6;
	
	int_to_char(tempo,value,10);
    lcd_cmd(LCD_FIRST_ROW);
    lcd_print(value);
    lcd_print("s ");
}

// Desabilidando a funcionalidade do botão reset para tarbalhar como um botão normal. 
#pragma vector = NMI_VECTOR 
__interrupt void nmi_isr(void)  { 
    if (IFG1 & NMIIFG) // nmi caused by nRST/NMI pin 
    { 
        IFG1 &= ~NMIIFG; 
        if (WDTCTL & WDTNMIES) // falling edge detected 
        { 
            if (tempo > 0) {
                tempo = tempo - 6;
            } else {
                tempo = 0;
            }
            int_to_char(tempo,value,10);
            lcd_cmd(LCD_FIRST_ROW);
            lcd_print(value);
            lcd_print("s ");
            S1_MAKE; // tell the rest of the world that s1 is depressed 
            WDTCTL = WDT_MDLY_32 | WDTNMI; // debounce and detect rising edge 
        } 
        else // rising edge detected 
        { 
            S1_BREAK; // tell the rest of the world that s1 is released 
            WDTCTL = WDT_MDLY_32 | WDTNMIES | WDTNMI; // debounce and detect falling edge 
        } 
    } // Note that NMIIE is cleared; wdt_isr will set NMIIE 32msec later 
    else {} 
} 

// WDT is used exclusively to debounce s1 and re-enable NMIIE 
#pragma vector = WDT_VECTOR 
__interrupt void wdt_isr(void) 
{ 
    WDTCTL ^= FLIP_HOLD; // Flip the HOLD bit only, others remain unchanged 
    IFG1 &= NMIIFG; // It may have been set by switch bouncing, clear it 
    IE1 |= NMIIE; // Now we can enable nmi to detect the next edge 
} 

/* 
* function to reverse a string  
*/
void my_reverse(char str[], int len) {
    int start, end;
    char temp;
    for(start=0, end=len-1; start < end; start++, end--) {
        temp = *(str+start);
        *(str+start) = *(str+end);
        *(str+end) = temp;
    }
}

/* 
* function int to char  
*/ 
char* int_to_char(int num, char* str, int base) {
    int i = 0;
    int isNegative = 0;
  
    /* A zero is same "0" string in all base */
    if (num == 0) {
        str[i] = '0';
        str[i + 1] = '\0';
        return str;
    }
  
    /* negative numbers are only handled if base is 10 
       otherwise considered unsigned number */
    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }
  
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'A' : rem + '0';
        num = num/base;
    }
  
    /* Append negative sign for negative numbers */
    if (isNegative == 1){
        str[i++] = '-';
    }
  
    str[i] = '\0';
 
    my_reverse(str, i);
  
    return str;
}