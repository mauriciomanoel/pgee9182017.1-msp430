#include <msp430.h>
#include <lcd.h>

unsigned int valueADC=0,i;	//variável que vai receber a conversão AD
char valueAnalog[5];		//string que receberá a conversão do valor inteiro para string
int mediaValueACD[20];      // Array de Inteiros para armazenar valores do ACD10
int value;                  //variável temporaria
int volt;                   //variável para armazenar a média da voltagem
unsigned int contador = 0;  //contador de valores do ACD10

// Function prototypes
void ConfigureAdc(void);
void my_reverse(char str[], int len);
char* int_to_char(int num, char* str, int base);
char* concatenate_string(char* str1, char* str2);

/*
 * main.c
 */
void main(void) {
	WDTCTL = WDTPW + WDTHOLD;		//Desabilita Watchdog Timer Counter
	BCSCTL1 = CALBC1_1MHZ;			//Set range   DCOCTL = CALDCO_1MHZ;
	BCSCTL2 &= ~(DIVS_3);			//SMCLK = DCO = 1MHz
	P1SEL |= BIT6;					//Seleciona a porta 4 para conversão
	P1DIR |= 0x01;                  // P1.0 output
	
    TACTL = TASSEL_2 + MC_1 + ID_3;       // TA0 CTL = 1011 01000
    CCTL0 = CCIE;                         // TA0 CCTL0
    CCR0 = 12099;                         // TA0 CCR0 value is 24198 (0x5E86) 0.999s
    
    lcd_init();						//Função que inicializa LCD
    initValueACD();                 //Inicializando o array de inteiros do conversor ACD;
	ConfigureAdc();					//Configura conversor ADC
	__enable_interrupt();			//Habilita interrupção
	
	lcd_print("VOLTAGEM");          //Escreve no LCD
	lcd_cmd(LCD_JUMP_LINE);			//Comando para pular linha no LCD
	lcd_print(":");                 
	while(1) {
		__delay_cycles(1000);				//Espera estabilização dos valores de referência
		__bis_SR_register(CPUOFF + GIE);	//Low Power Mode 0 com interrupção para o conversor habilitada
		
	    // Pega a quantidade de amostras do conversor ACD
		if (contador > 10) {
    		value = calcularMedia();            //Calcular a média
    		volt = (value*0.0032258) * 100;     //Faz a conversão, já que a voltagem no MSP é  ADC10MEM * (~3,56v / 1023)
    		int_to_char(volt,valueAnalog,10);	//Converte valor inteiro para string
    		lcd_cmd(LCD_ADVANCE_COLUMN);		//Avança uma coluna no LCD
    		lcd_print("      ");
    		lcd_cmd(LCD_ADVANCE_COLUMN);        // Avançar uma coluna no LCD
    		lcd_print(valueAnalog);			    //Escreve string com o valor inteiro da conversão no LCD
    		contador = 0;
		}
		
		for(i=0;i<500;i++) {
			__delay_cycles(1000);
		}
		
	}
}

// Timer A0: Timer que guarda os valores do ACD10MEM
#pragma vector=TIMER0_A0_VECTOR
__interrupt void badprog_timer_a0(void) {
  
  P1OUT ^= 0x01;                        // Toggle P1.0
  ADC10CTL0 |= ENC + ADC10SC;			// Começa a amostragem e conversão
  mediaValueACD[contador] = ADC10MEM;   
  contador = contador + 1;
}

// ADC10 rotina de interrupção
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void) {
	__bic_SR_register_on_exit(CPUOFF);        //Volta ao modo ativo;
}

//Configuração do conversor
void ConfigureAdc(void) {
	ADC10CTL1 = INCH_6 + ADC10DIV_3 ;						//Canal 6 e clock do ADC dividido por 3
	ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;  	//Vcc & Vss como referencia, Sample and hold com 64 ciclos de clock, ADC on, interrupção do ADC habilitada
	ADC10AE0 |= BIT6;									    //Entrada ADC habilitada P1.6
}

/* 
* Função para reverter uma string: Auxiliar da função int_to_char
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
* Converter inteiro para char  
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
  
    /* negative numbers are only handled if base is 10 otherwise considered unsigned number */
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

/* 
* Incializando o array de inteiro
*/ 
int initValueACD() {
    for(i=0; i<20; i++) {
        mediaValueACD[i] = 0;
    }
}

/* 
* Calcular a média de valores ACD
*/ 
int calcularMedia() {
    float media = 0;
    int valor = 0;
    for(i=0; i<contador; i++) {
        if (mediaValueACD[i] != 0) {
            media += mediaValueACD[i];
            valor++;
        }
    }
    
    if (valor == 0) return 0;
    return (media / valor);
}