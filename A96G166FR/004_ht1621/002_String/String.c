/**---------------------------------------------
*\date  07.05.2023
*\brief
* HT1621 example String
*\brief
***********************************************************************************************
* В данном примере подключим, инициализируем дисплей и выведем на дисплей значения на экран от 0 до 9.
* Также выведем на дисплей пробел , * , _ , - , |.
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

#include    "Intrins.h"
#include    "delay.h"     
#include "a96g166_gpio.h"
#include "a96g166_clock.h"
#include "a96g166_bit.h"
//#include "math.h"
#include "string.h"


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

char character;

void Delay (u16 ms);
void lcd_begin(void);
void config(void);
void wrCMD(unsigned char CMD);
void wrDATA(unsigned char Data, unsigned char cnt);
void backlight(void);
void clear(void);
void wrCLR(unsigned char len);
void wrclrdata(unsigned char addr, unsigned char sdata);
void print_str(const char* str, bool leftPadded);
char charToSegBits(char character);
void update(void);
void wrone(unsigned char addr, unsigned char sdata);
void setdecimalseparator(int decimaldigits);

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
    print_str("012",1);
    Delay(1000);
    print_str("012",0);
    Delay(1000);
    print_str("543210");
    Delay(1000);
    print_str("6789");
    Delay(1000);
    print_str("* | -_");
    Delay(1000);
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
  Delay_us(4);//delayMicroseconds(4);
  if (Data & 0x80) {
    Port_SetOutputHighpin(PORT1, PIN2);//digitalWrite(_data_p, HIGH);
  }
  else {
    Port_SetOutputLowpin (PORT1, PIN2);//digitalWrite(_data_p, LOW);
  }
  Port_SetOutputHighpin(PORT1, PIN1);//digitalWrite(_wr_p, HIGH);
  Delay_us(4);//delayMicroseconds(4);
  Data <<= 1;
  }
}
//*************************************//
void backlight(void)
{
  if (_backlight_en)
   Port_SetOutputHighpin(PORT1, PIN3);//digitalWrite(_backlight_p, HIGH);
   Delay_us(4);
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
void print_str(const char* str, bool leftPadded){
  int chars = strlen(str);
  int padding = 6 - chars;

  for( i = 0; i < 6; i++){
    _buffer[i] &= 0x80; // mask the first bit, used by batter and decimal point
  character = leftPadded
               ? i < padding ? ' ' : str[i - padding]
               : i >= chars ? ' ' : str[i];
    _buffer[i] |= charToSegBits(character);
  }
  setdecimalseparator(0);
  update();
}
//*************************************//
void setdecimalseparator(int decimaldigits) {
  _buffer[3] &= 0x7F;
  _buffer[4] &= 0x7F;
  _buffer[5] &= 0x7F;

  if( decimaldigits <= 0 || decimaldigits > 3){
    return;
  }
  _buffer[6-decimaldigits] |= 0x80;
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
char charToSegBits(char character) {
  switch (character) {
  case '0':
    return 0x7D;//0b1111101;
  case '1':
    return 0x60;//0b1100000;
  case '2':
    return 0x3E;//0b111110;
  case '3':
    return 0x7A;//0b1111010;
  case '4':
    return 0x63;//0b1100011;
  case '5':
    return 0x5B;//0b1011011;
  case '6':
    return 0x5F;//0b1011111;
  case '7':
    return 0x70;//0b1110000;
  case '8':
    return 0x7F;//0b1111111;
  case '9':
    return 0x7B;//0b1111011;
  case ' ':
    return 0x00;//0b1111011;
  case '*': 
    return 0x33;//0b0110011;
  case '|':
    return 0x5;//0b0000101;
  case '-':
    return 0x2;//0b0000010;
  case '_':
    return 0x8;//0b0001000;
  }
}
//**************END********************//
