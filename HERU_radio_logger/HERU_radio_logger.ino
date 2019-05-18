#include "NRF905.h"
#include <SPI.h>
#define BUF_LEN          32
#define CONF_LEN         10

//NRF905 nrf905;

unsigned char rx_buf[100]= {0};
unsigned char read_config_buf[CONF_LEN];

void setup()
{
    char i;
    /** serial communication configurate */
    Serial.begin(9600);
    //pinMode(10,OUTPUT);
    nrf905=NRF905();
    
    /** pin/port configuration */
    Serial.print("init");
    nrf905.init(); 

  
    /***************************************************
        default configuration, need to specify frequency
        choose Z-Wave frequency band, support :
        US		908.42Mhz
        EUROPE		868.42MHz
        AFRICA		868.42MHz
        CHINA		868.42MHz
        HK		919.82MHz
        JAPAN		853.42MHz
        AUSTRALIA	921.42MHz
        NEW_ZEALAND	921.42MHz
        BRASIL		921.42MHz
        RUSSIA		896MHz
    */ 
     
	nrf905.write_config(EUROPE); 	
    
    /***********************************************************
	read register configuration, check register value written */
    Serial.print("read config");
    nrf905.read_config(read_config_buf);
    
        /** test configuration */
    for(i=0; i<CONF_LEN; i++){
        Serial.print(read_config_buf[i],HEX);
        Serial.print(' ');
    }
    Serial.println(' ');
}

void loop()
{
    char i;
         
    if(nrf905.RX(rx_buf)==1)
	{
		for(i=0; i<=rx_buf[0]; i++){
		Serial.print("0x");
        Serial.print(rx_buf[i],HEX);
        Serial.print(",");
    }
	Serial.println(" ");
	}
	
}
