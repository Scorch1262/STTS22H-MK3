#define FCY 16000000UL 
#include <assert.h>
#include <stdbool.h>
#include "mcc_generated_files/system.h"
#include <stdio.h>
#include <stdlib.h>
#include <libpic30.h>
#include <xc.h>
#include <string.h>
#include "mcc_generated_files/i2c1.h"
#include "stts22h.h"

#if defined(__dsPIC33E__)
	#include <p33exxxx.h>
#elif defined(__dsPIC33F__)
	#include <p33fxxxx.h>
#elif defined(__dsPIC30F__)
	#include <p30fxxxx.h>
#elif defined(__PIC24E__)
	#include <p24exxxx.h>
#elif defined(__PIC24H__)
	#include <p24hxxxx.h>
#elif defined(__PIC24F__) || defined(__PIC24FK__)
	#include <p24fxxxx.h>
#endif
   
float temperatur;                               // definiert "temperatur"

/*
                         Main application
 */
int main(void){                                 // "main"
    SYSTEM_Initialize();                        // ruft "SYSTEM_Initialize" auf
    STTS22H_Init();                             // ruft "STTS22H_Init" auf
    
    while (1){                                  // Endloschleife
        temperatur = STTS22H_GetValue();        // schreibt ergebnis von "STTS22H_GetValue" in "temperatur"
        printf("Temp = %4.2fC\r\n",temperatur);  // schreibt "Temp = %.2fC\r\n" an UART           
        __delay_ms(500);                        // warte 500ms
    }                                           // 
}                                               // 

