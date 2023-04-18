/**---------------------------------------------
*\date  18.04.2023
*\brief
* HT1621 example Battery levels
*\brief
***********************************************************************************************
* В данном примере подключим, инициализируем дисплей и выведем три уровня заряда батареи на LCD.
***********************************************************************************************
*     A96G166FR                   HT1621
*   ------------                -----------
*  |            |              |
*  |        P1.0|------------->| CS
*  |        P1.1|------------->| WR 
*  |        P1.2|------------->| DATA
*  |        P1.3|------------->| LED+(EN)
*  |            |              | Vcc --> +5V!!!
*  |            |              | GND
*
* Код взят и адаптирован для A96G166FR отсюда.
* LCD Arduino library https://github.com/valerionew/ht1621-7-seg
*
* HT1621 – это LCD драйвер, способный управлять 128 элементным (32х4) индикатором. 
* Возможность конфигурирования делает HT1620 пригодным для использования во множестве LCD устройств, включая LCD модули и системы дисплеев.
* Микросхема при формировании напряжения смещения для питания LCD потребляет совсем небольшой ток.
*
*\ author ScuratovaAnna 
*\ сode debugging PivnevNikolay
*/

#include "Intrins.h"
#include "delay.h"     
#include "a96g166_gpio.h"
#include "a96g166_clock.h"
#include "a96g166_bit.h"
//#include "math.h"
//#include "string.h"

#define  BIAS     0x52
#define  SYSDIS   0X00
#define  SYSEN    0X02
#define  LCDOFF   0X04
#define  LCDON    0X06
#define  XTAL     0x28
#define  RC256    0X30
#define  TONEON   0X12
#define  TONEOFF  0X10
#define  WDTDIS1  0X0A

int _buffer[7] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,};

volatile u16 count;
bit _backlight_en;
u8 i;

void Delay (u16 ms);
void lcd_begin(void);
void config(void);
void wrCMD(unsigned char CMD);
void wrDATA(unsigned char Data, unsigned char cnt);
void backlight(void);
void clear(void);
void wrCLR(unsigned char len);
void wrclrdata(unsigned char addr, unsigned char sdata);
void update(void);
void wrone(unsigned char addr, unsigned char sdata);
void setBatteryLevel(int level);

void BIT_Int_Handler(void) interrupt BIT_VECT
{
  if (count){count --;}
  BIT_ClearInterruptStatus();
}

void Main(void)
{
  GLOBAL_INTERRUPT_DIS();   
  Port_Initial();
  Clock_Initial(HSI32_DIV2);//16Mhz
  GLOBAL_INTERRUPT_EN();
//*************************************//
  lcd_begin();
  backlight();
  clear();
  while(1)
  {
    setBatteryLevel(1);
    Delay(200);
    setBatteryLevel(2);
    Delay(200);
    setBatteryLevel(3);
    Delay(200);
    setBatteryLevel(0);
    Delay(200);
  }
}

void Delay (u16 ms){
  BIT_Initial(BIT_DIV128, BIT_X128);   
  BIT_Interrupt_config(TRUE);
  count = ms;
  while(count);
}
//*************************************//
void lcd_begin(void)
 {
  //lcd_begin(13,12,8,10); --> (cs, wr, Data, backlight)
  Port_SetOutputpin(PORT1, PIN0, PUSH_PULL);//cs_p
  Port_SetOutputpin(PORT1, PIN1, PUSH_PULL);//wr_p
  Port_SetOutputpin(PORT1, PIN2, PUSH_PULL);//data_p
  Port_SetOutputpin(PORT1, PIN3, PUSH_PULL);//backlight_p
  _backlight_en = TRUE;
  config();
}
//*************************************//
void config(void)
 {
  wrCMD(BIAS);
  wrCMD(RC256);
  wrCMD(SYSDIS);
  wrCMD(WDTDIS1);
  wrCMD(SYSEN);
  wrCMD(LCDON);
}
 //*************************************//
void wrCMD(unsigned char CMD) {
  Port_SetOutputLowpin (PORT1, PIN0);//digitalWrite(_cs_p, LOW);
  wrDATA(0x80, 4);
  wrDATA(CMD, 8);
  Port_SetOutputHighpin(PORT1, PIN0);//digitalWrite(_cs_p, HIGH);
}
//*************************************//
void wrDATA(unsigned char Data, unsigned char cnt) {
  unsigned char i;
  for (i = 0; i < cnt; i++) {
  Port_SetOutputLowpin (PORT1, PIN1);//digitalWrite(_wr_p, LOW);
  Delay(1);//delayMicroseconds(4);
  if (Data & 0x80) {
    Port_SetOutputHighpin(PORT1, PIN2);//digitalWrite(_data_p, HIGH);
  }
  else {
    Port_SetOutputLowpin (PORT1, PIN2);//digitalWrite(_data_p, LOW);
  }
  Port_SetOutputHighpin(PORT1, PIN1);//digitalWrite(_wr_p, HIGH);
  Delay(1);//delayMicroseconds(4);
  Data <<= 1;
  }
}
//*************************************//
void backlight(void)
{
  if (_backlight_en)
   Port_SetOutputHighpin(PORT1, PIN3);//digitalWrite(_backlight_p, HIGH);
  Delay(1);
}
//*************************************//
void clear(void){
  wrCLR(16);
}
//*************************************//
void wrCLR(unsigned char len) {
  unsigned char addr = 0;
  unsigned char i;
  for (i = 0; i < len; i++) {
   wrclrdata(addr, 0x00);
   addr = addr + 2;
  }
}
//*************************************//
void wrclrdata(unsigned char addr, unsigned char sdata)
{
  addr <<= 2;
  Port_SetOutputLowpin (PORT1, PIN0);//digitalWrite(_cs_p, LOW);
  wrDATA(0xa0, 3);
  wrDATA(addr, 6);
  wrDATA(sdata, 8);
  Port_SetOutputHighpin(PORT1, PIN0);//digitalWrite(_cs_p, HIGH);
}
//*************************************//
void update(void){
  wrone(0, _buffer[5]);
  wrone(2, _buffer[4]);
  wrone(4, _buffer[3]);
  wrone(6, _buffer[2]);
  wrone(8, _buffer[1]);
  wrone(10,_buffer[0]);
}
//*************************************//
void wrone(unsigned char addr, unsigned char sdata)
{
  addr <<= 2;
  Port_SetOutputLowpin (PORT1, PIN0);//digitalWrite(_cs_p, LOW);
  wrDATA(0xa0, 3);
  wrDATA(addr, 6);
  wrDATA(sdata, 8);
  Port_SetOutputHighpin(PORT1, PIN0);//digitalWrite(_cs_p, HIGH);
}
//*************************************//
void setBatteryLevel(int level) {
  // zero out the previous (otherwise the or couldn't be possible)
  _buffer[0] &= 0x7F;
  _buffer[1] &= 0x7F;
  _buffer[2] &= 0x7F;

  switch(level){
    case 3: // battery on and all 3 segments
     _buffer[0] |= 0x80;
    case 2: // battery on and 2 segments
     _buffer[1] |= 0x80;
    case 1: // battery on and 1 segment
     _buffer[2] |= 0x80;
    case 0: // battery indication off
    default:
   break;
  }
update();
}
//**************END********************//
