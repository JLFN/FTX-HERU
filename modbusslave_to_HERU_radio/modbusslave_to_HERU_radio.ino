
#include <ESP8266WiFi.h>
#include "NRF905.h"
#include <SPI.h>

// NRF905 stuff

#define BUF_LEN          32
#define CONF_LEN         10
 
unsigned char rx_buf[100]= {0};
unsigned char read_config_buf[CONF_LEN];

int temp_uteluft_in;
int temp_avluft_ut;
int temp_tilluft_hus;
int temp_franluft_hus;
int flakthastighet_tilluft;
int flakthastighet_franluft;
unsigned char bortalage;
unsigned char forcering;

// Commands sent from remote control to Heru

unsigned char command_0x01_0x9A[12]={0x0B,0xCC,0x64,0x4A,0xB0,0x04,0xD9,0xA9,0x01,0x9A,0x86,0x58};
unsigned char command_0x01_0x9B[12]={0x0B,0xCC,0x64,0x4A,0xB0,0x04,0xD9,0xA9,0x01,0x9B,0x96,0x79};
unsigned char command_away_mode_on[13]={0x0C,0xCC,0x64,0x4A,0xB0,0x04,0xD9,0xA9,0x02,0x41,0x01,0x29,0xE2};
unsigned char command_away_mode_off[13]={0x0C,0xCC,0x64,0x4A,0xB0,0x04,0xD9,0xA9,0x02,0x41,0x00,0x39,0xC3}; 
unsigned char command_force_on[13]={0x0C,0xCC,0x64,0x4A,0xB0,0x04,0xD9,0xA9,0x02,0x40,0x01,0x1A,0xD3};
unsigned char command_force_off[13]={0x0C,0xCC,0x64,0x4A,0xB0,0x04,0xD9,0xA9,0x02,0x40,0x00,0x0A,0xF2};
 
 // MODBUS tcp stuff
const char* ssid = "Saralisasv.74";
const char* password = "--------";
int ModbusTCP_port = 502;
 
//////// Required for Modbus TCP / IP /// 
#define maxInputRegister 20
#define maxHoldingRegister 20
 
#define MB_FC_NONE 0
#define MB_FC_READ_REGISTERS 3 //implemented
#define MB_FC_WRITE_REGISTER 6 //implemented
#define MB_FC_WRITE_MULTIPLE_REGISTERS 16 //implemented
//
// MODBUS Error Codes
//
#define MB_EC_NONE 0
#define MB_EC_ILLEGAL_FUNCTION 1
#define MB_EC_ILLEGAL_DATA_ADDRESS 2
#define MB_EC_ILLEGAL_DATA_VALUE 3
#define MB_EC_SLAVE_DEVICE_FAILURE 4
//
// MODBUS MBAP offsets
//
#define MB_TCP_TID 0
#define MB_TCP_PID 2
#define MB_TCP_LEN 4
#define MB_TCP_UID 6
#define MB_TCP_FUNC 7
#define MB_TCP_REGISTER_START 8
#define MB_TCP_REGISTER_NUMBER 10
 
byte ByteArray[260];
unsigned int MBHoldingRegister[maxHoldingRegister];
 
//////////////////////////////////////////////////////////////////////////
  
WiFiServer MBServer(ModbusTCP_port);

// Decode recieved radio messagelength
void decode(unsigned char *rx_buf)
{
   int command;
   command=rx_buf[8]<<8|rx_buf[9];
   Serial.print("Response command from HERU: ");
   Serial.println(command,HEX);
   switch(command)
   {
    case 0x159B:
    // Spara temperaturer
    temp_uteluft_in=rx_buf[10]<<8|rx_buf[11];
    //if(temp_uteluft_in>0x7FFF) temp_uteluft_in=temp_uteluft_in-0xFFFF;
    temp_franluft_hus=rx_buf[12]<<8|rx_buf[13];
    temp_tilluft_hus=rx_buf[14]<<8|rx_buf[15];
    temp_avluft_ut=rx_buf[16]<<8|rx_buf[17];
    //if(temp_avluft_ut>0x7FFF) temp_avluft_ut=temp_avluft_ut-0xFFFF;
    
    /*Serial.print("Temperatur: ");
    Serial.println((float)temp_uteluft_in/10);
    Serial.println((float)temp_avluft_ut/10);
    Serial.println((float)temp_tilluft_hus/10);
    Serial.println((float)temp_franluft_hus/10);*/
    break;
    
    case 0x159A:
    flakthastighet_tilluft=rx_buf[26]<<8|rx_buf[27];
    flakthastighet_franluft=rx_buf[28]<<8|rx_buf[29];
    bortalage=rx_buf[19];
    forcering=rx_buf[17];
    /*Serial.println("Flakthastighet Till,Från,Bortlaläge(08=till 00=från)");
    Serial.println(flakthastighet_tilluft);
    Serial.println(flakthastighet_franluft);
    Serial.println(bortalage);*/
    break;
    }

}
 
void setup() {
 
 // Setup modbus
 pinMode(14, OUTPUT);
 
 Serial.begin(9600);
 delay(100) ;
 WiFi.begin(ssid, password);
 delay(100) ;
 Serial.println(".");
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
 MBServer.begin();
 Serial.println("Connected ");
 Serial.print("ESP8266 Slave Modbus TCP/IP ");
 Serial.print(WiFi.localIP());
 Serial.print(":");
 Serial.println(String(ModbusTCP_port));
 Serial.println("Modbus TCP/IP Online");
 
 
 // Setup NRF905
 char i;
   
    //pinMode(10,OUTPUT);
    nrf905=NRF905();
    
    /** pin/port configuration */
    Serial.print("init NRF905");
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
    Serial.print("Read NRF905 config: ");
    nrf905.read_config(read_config_buf);
        
    /** Print configuration */
    for(i=0; i<CONF_LEN; i++){
        Serial.print(read_config_buf[i],HEX);
        Serial.print(' ');
    }
    Serial.println(' ');
}
 
 
void loop() {
  
 // Check if a client has connected // Modbus TCP/IP
 WiFiClient client = MBServer.available();
 if (!client) {
 return;
 }
 
 
 boolean flagClientConnected = 0;
 byte byteFN = MB_FC_NONE;
 int Start;
 int WordDataLength;
 int ByteDataLength;
 int MessageLength;
 
 // Modbus TCP/IP
 while (client.connected())
 {
  if(client.available())
  {
    flagClientConnected = 1;
    int i = 0;
    while(client.available())
    {
      ByteArray[i] = client.read();
      i++;
    }
 
  client.flush();
 
 ////////////////////////////////
 // NRF905
	nrf905.TX(command_0x01_0x9B);
  delay(100);
	if(nrf905.RX(rx_buf)==1) decode(rx_buf);
	nrf905.TX(command_0x01_0x9A);
  delay(100);
  if(nrf905.RX(rx_buf)==1) decode(rx_buf);


 ///////// Holding Register [0] A [9] = 10 Holding Registers Writing
  
 MBHoldingRegister[0] = temp_uteluft_in;
 MBHoldingRegister[1] = temp_avluft_ut;
 MBHoldingRegister[2] = temp_tilluft_hus;
 MBHoldingRegister[3] = temp_franluft_hus;
 MBHoldingRegister[4] = flakthastighet_tilluft;
 MBHoldingRegister[5] = flakthastighet_franluft;
 MBHoldingRegister[6] = bortalage;
 MBHoldingRegister[7] = forcering;
 /*MBHoldingRegister[8] = 0;
 MBHoldingRegister[9] = 0;*/
 
 
 
 
  ///// Holding Register [10] A [19] = 10 Holding Registers Reading
 
 /*int Temporal[10];
 
 Temporal[0] = MBHoldingRegister[10];
 Temporal[1] = MBHoldingRegister[11];
 Temporal[2] = MBHoldingRegister[12];
 Temporal[3] = MBHoldingRegister[13];
 Temporal[4] = MBHoldingRegister[14];
 Temporal[5] = MBHoldingRegister[15];
 Temporal[6] = MBHoldingRegister[16];
 Temporal[7] = MBHoldingRegister[17];
 Temporal[8] = MBHoldingRegister[18];
 Temporal[9] = MBHoldingRegister[19];
 
 //// debug
 
 for (int i = 0; i < 10; i++) 
 {
  Serial.print("[");
  Serial.print(i);
  Serial.print("] ");
  Serial.print(Temporal[i]);
 }
 Serial.println("");*/
 
 
 
 
//// end code - fin 
 
 
 //// rutine Modbus TCP
 byteFN = ByteArray[MB_TCP_FUNC];
 Start = word(ByteArray[MB_TCP_REGISTER_START],ByteArray[MB_TCP_REGISTER_START+1]);
 WordDataLength = word(ByteArray[MB_TCP_REGISTER_NUMBER],ByteArray[MB_TCP_REGISTER_NUMBER+1]);
 }
 
 // Handle request
 
 switch(byteFN) {
 case MB_FC_NONE:
 break;
 
 case MB_FC_READ_REGISTERS: // 03 Read Holding Registers
 ByteDataLength = WordDataLength * 2;
 ByteArray[5] = ByteDataLength + 3; //Number of bytes after this one.
 ByteArray[8] = ByteDataLength; //Number of bytes after this one (or number of bytes of data).
 for(int i = 0; i < WordDataLength; i++)
 {
 ByteArray[ 9 + i * 2] = highByte(MBHoldingRegister[Start + i]);
 ByteArray[10 + i * 2] = lowByte(MBHoldingRegister[Start + i]);
 }
 MessageLength = ByteDataLength + 9;
 client.write((const uint8_t *)ByteArray,MessageLength);
 
 byteFN = MB_FC_NONE;
 
 break;
 
 
 case MB_FC_WRITE_REGISTER: // 06 Write Holding Register
 MBHoldingRegister[Start] = word(ByteArray[MB_TCP_REGISTER_NUMBER],ByteArray[MB_TCP_REGISTER_NUMBER+1]);
 ByteArray[5] = 6; //Number of bytes after this one.
 MessageLength = 12;
 client.write((const uint8_t *)ByteArray,MessageLength);
 byteFN = MB_FC_NONE;

 switch(Start)
 {
  case 10:
  if(MBHoldingRegister[Start]==01)
  {
    nrf905.TX(command_away_mode_on);
    Serial.println("BORTALAGE till!!!");
  }
  if(MBHoldingRegister[Start]==00)
  {
    nrf905.TX(command_away_mode_off);
    Serial.println("BORTALAGE av!!!");
  }
  break;
  case 11:
  if(MBHoldingRegister[Start]==01)
  {
    nrf905.TX(command_force_on);
    Serial.println("Forcering till!!!");
  }
  if(MBHoldingRegister[Start]==00)
  {
    nrf905.TX(command_force_off);
    Serial.println("Forcering av!!!");
  }
  break;
 }
 
 break;
 
 case MB_FC_WRITE_MULTIPLE_REGISTERS: //16 Write Holding Registers
 ByteDataLength = WordDataLength * 2;
 ByteArray[5] = ByteDataLength + 3; //Number of bytes after this one.
 for(int i = 0; i < WordDataLength; i++)
 {
 MBHoldingRegister[Start + i] = word(ByteArray[ 13 + i * 2],ByteArray[14 + i * 2]);
 }
 MessageLength = 12;
 client.write((const uint8_t *)ByteArray,MessageLength); 
 byteFN = MB_FC_NONE;
 
 break;
 }
 }
 
 
 
 
}
