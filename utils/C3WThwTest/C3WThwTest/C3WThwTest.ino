//#include <Audio.h>
//#include "SM2k_control_wm8731.h"
#include <SPI.h>
#include <RA8875.h>
#include <Wire.h>
//#include <FT5206.h>

#define RA8875_CS 10   // ER-TFTM070 pin 5
#define RA8875_RESET 9 // ER-TFTM070 pin 11
#define RA8875_MOSI 11  //ER-TFTM070 pin 7
#define RA8875_MISO 12  //ER-TFTM070 pin 6
#define RA8875_SCLK 13   //ER-TFTM070 pin 8

#define TP_INT 32 // touch data ready for read from FT5206 touch controller
//#define TP_WAKE 32

#define FT5206_I2C_ADDRESS 0x38
#define FT5206_REGISTERS 12

// AudioOutputI2S        output_i2s;
// AudioSynthWaveform             osc;
// AudioConnection p1(osc, 0, output_i2s , 0);
// AudioConnection p2(osc, 0, output_i2s , 1);
// SM2k_AudioControlWM8731 codecControl1;

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);
//FT5206 cts = FT5206(CTP_INT);

uint8_t registers[FT5206_REGISTERS];
uint16_t new_coordinates[5][2];
uint8_t current_touches = 0;
uint8_t old_touches = 0;

void setup()
{
  pinMode(TP_INT, INPUT_PULLDOWN);

  attachInterrupt(digitalPinToInterrupt(TP_INT), getRegisters, CHANGE);

  Serial.println("BEGIN");
  // AudioMemory(32);
  // codecControl1.setAddress(0x1A);
  // codecControl1.enable();
  // codecControl1.inputLevel(0.0);
  // osc.begin(1.0, 440.0, WAVEFORM_SINE);

  tft.begin(RA8875_800x480);
  tft.fillWindow(0xfe01);

  Wire1.begin();
  //writeRegister(FT5206_DEVICE_MODE,0);
  //cts.begin();
}

elapsedMillis timer = 0;

void loop()
{
  if (timer > 20)
  {
    timer = 0;
  //   //uint8_t temp = digitalReadFast(CTP_INT);
  //   getRegisters();
  // //   //uint8_t temp = getTSflag(registers);
  //   //Serial.println(temp);
  }
  //Wire1.beginTransmission(0x38);

  //Wire1.endTransmission();
  // if (cts.touched())
  // {
  //   cts.getTSregisters(registers);
  //   current_touches = cts.getTScoordinates(new_coordinates, registers);
  //   if (current_touches > 0) Serial.printf("x: %d, y: %d\n", new_coordinates[0], new_coordinates[1]);
  // }
}

void getRegisters()
{
  Wire1.beginTransmission(FT5206_I2C_ADDRESS);
  Wire1.requestFrom(FT5206_I2C_ADDRESS, FT5206_REGISTERS); 
  uint8_t register_number = 0;
  while(Wire1.available())
  {
    //uint8_t register_number = 0;
    registers[register_number] = Wire1.read();
    //Serial.print(registers[register_number]);
    //Serial.print(", ");
    register_number++;
  }
  Serial.println("");
  Wire1.endTransmission(FT5206_I2C_ADDRESS);
}

// uint8_t getTSflag(uint8_t *reg) 
// {
// 	uint8_t temp = reg[FT5206_TOUCH1_XH];
// 	//if (!bitRead(temp,7) && !bitRead(temp,6)) return 3;//
// 	if (!bitRead(temp,7) && bitRead(temp,6)) return 1;//finger up
// 	if (bitRead(temp,7) && !bitRead(temp,6)) return 2;//finger down
// 	return 0;
// }

// void writeRegister(uint8_t reg,uint8_t val)
// {
//     Wire1.beginTransmission(FT5206_I2C_ADDRESS);
//     Wire1.write(reg);
//     Wire1.write(val);
//     Wire1.endTransmission(FT5206_I2C_ADDRESS);
// }
