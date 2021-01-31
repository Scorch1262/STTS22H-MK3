#define FCY 16000000UL

#include "xc.h"
#include <assert.h>
#include <stdbool.h>
#include <libpic30.h>
#include "mcc_generated_files/system.h"
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

#define STTS22H_ADDR    0x3F 
    
#define WHOAMI          0x01
#define TEMP_H_LIMIT    0x02
#define TEMP_L_LIMIT    0x03
#define CTRL            0x04
#define STATUS          0x05
#define TEMP_L_OUT      0x06
#define TEMP_H_OUT      0x07
#define SOFTWARE_RESET  0x0c

I2C1_MESSAGE_STATUS status;                                              // 

uint8_t Slave_Address;                                                   // definiert "Slave_Address" 

uint8_t  writeBuffer[3] = {0x00, 0x00, 0x00};                            // definiert "writeBuffer"
uint8_t  readBuffer[2] = {0x00, 0x00};                                   // definiert "readBuffer" 
uint16_t retryTimeOut1;                                                  // definiert "retryTimeOut1" 
uint16_t retryTimeOut2;                                                  // definiert "retryTimeOut2" 

void STTS22H_Init(void){                                                // "STTS22H_Init"
    Slave_Address = STTS22H_ADDR;                                       // schreibt "STTS22H_ADDR" in "Slave_Address"

    writeBuffer[0] = SOFTWARE_RESET;                                    // schreibt "SOFTWARE_RESET" in "writeBuffer[0]"
    writeBuffer[1] = 0x30;                                              // schreibt "0x30" in "writeBuffer[1]"
    I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);       // schreibt 2 Byte (writeBuffer[0]) an I2C1
    i2c1_message_pending_100();                                         // ruft "i2c1_message_pending_100" auf 
    __delay_ms(500);                                                    // warte 500ms
}                                                                       // 

float STTS22H_GetValue(void){                                           // "STTS22H_GetValue"        
    Slave_Address = STTS22H_ADDR;                                       // schreibt "STTS22H_ADDR" in "Slave_Address"
    
    uint8_t Data;                                                       // definiert "Data"
    uint16_t tempData;                                                  // definiert "tempData"
    float tempDataAnz;                                                  // definiert "tempDataAnz" 
    
    writeBuffer[0] = SOFTWARE_RESET;                                    // schreibt "SOFTWARE_RESET" in "writeBuffer[0]"
    writeBuffer[1] = 0x02;                                              // schreibt "0x02" in "writeBuffer[1]"
    I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);       // schreibt 2 Byte (writeBuffer[0]) an I2C1
    i2c1_message_pending_100();                                         // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = SOFTWARE_RESET;                                    // schreibt "SOFTWARE_RESET" in "writeBuffer[0]"
    writeBuffer[1] = 0x00;                                              // schreibt "0x00" in "writeBuffer[1]"
    I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);       // schreibt 2 Byte (writeBuffer[0]) an I2C1
    i2c1_message_pending_100();                                         // ruft "i2c1_message_pending_100" auf
    
    writeBuffer[0] = CTRL;                                              // schreibt "CTRL" in "writeBuffer[0]"
    I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);       // schreibt 1 Byte (writeBuffer[0]) an I2C1                  
    i2c1_message_pending_100();                                         // ruft "i2c1_message_pending_100" auf 
    I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);         // liest von I2C1 in 1 Byte (readBuffer[0]) 
    i2c1_message_pending_100();                                         // ruft "i2c1_message_pending_100" auf

    Data = readBuffer[0] + 1;                                           // "Data" + 1
    
    writeBuffer[0] = CTRL;                                              // schreibt "SOFTWARE_RESET" in "writeBuffer[0]"
    writeBuffer[1] = Data;                                              // schreibt "0x02" in "writeBuffer[1]"
    I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);       // schreibt 2 Byte (writeBuffer[0]) an I2C1
    i2c1_message_pending_100();                                         // ruft "i2c1_message_pending_100" auf
    
    writeBuffer[0] = TEMP_L_OUT;                                        // schreibt "TEMP_L_OUT" in "writeBuffer[0]"
    I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);       // schreibt 1 Byte (writeBuffer[0]) an I2C1                  
    i2c1_message_pending_100();                                         // ruft "i2c1_message_pending_100" auf 
    I2C1_MasterRead(&readBuffer[1], 1, Slave_Address, &status);         // liest von I2C1 in 1 Byte (readBuffer[1]) 
    i2c1_message_pending_100();                                         // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = TEMP_H_OUT;                                        // schreibt "TEMP_H_OUT" in "writeBuffer[0]"
    I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);       // schreibt 1 Byte (writeBuffer[0]) an I2C1                  
    i2c1_message_pending_100();                                         // ruft "i2c1_message_pending_100" auf 
    I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);         // liest von I2C1 in 1 Byte (readBuffer[0]) 
    i2c1_message_pending_100();                                         // ruft "i2c1_message_pending_100" auf
    
    if ((readBuffer[0] & 0x10) == 0x10){                                // wenn "readBuffer[0]" maskiert mit "0x10" gleich "0x10" ist 
        tempDataAnz = (readBuffer[0] << 8) + readBuffer[1] - 256;       // temp < 0C
    }else{                                                              // sonst
        tempDataAnz = (readBuffer[0] << 8) + readBuffer[1];             // temp > 0C      
    }                                                                   // 
    
    tempDataAnz = tempDataAnz / 100;                                    // "tempDataAnz" geteilt duch 100
    return tempDataAnz;                                                 // gibt "tempDataAnz" zurück     
}                                                                       // 

void i2c1_message_pending_100(void){                                    // "i2c1_message_pending_100"
    retryTimeOut1 = 0;                                                  // setzt "retryTimeOut1" auf 0
    retryTimeOut2 = 0;                                                  // setzt "retryTimeOut2" auf 0
    while(status == I2C1_MESSAGE_PENDING){                              // solange "status" gleich "I2C1_MESSAGE_PENDING" ist
        if (retryTimeOut2 == 500){                                      // wenn "retryTimeOut2" gleich 100 ist
            break;                                                      // Abbruch der Schleife
        }else{                                                          // sonst
            retryTimeOut2++;                                            // "retryTimeOut2" +1
        }                                                               // 
    }                                                                   // 
    while(status != I2C1_MESSAGE_FAIL){                                 // solange "status" nicht gleich "I2C1_MESSAGE_FAIL" ist
        if (retryTimeOut1 == 500){                                      // wenn "retryTimeOut1" gleich 100 ist
            break;                                                      // Abbruch der Schleife
        }else{                                                          // sonst
            retryTimeOut1++;                                            // "retryTimeOut1" +1
        }                                                               // 
    }                                                                   // 
}                                                                       // 