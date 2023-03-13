/**---------------------------------------------
*\date  13.03.2023
*\brief
* Basic Interval Timer
*\brief
*************************************************************************
* В данном примере светодиод мигает с интервалом в одну секунду.
*************************************************************************
*     A96G166FR
*   ------------
*  |            |
*  |        P1.0| --> LED 
*  |            |
*  |            |
*  |            |
*
* ARDUINO MULTI-FUNCTION SHIELD http://publicatorbar.ru/2017/12/21/arduino-multi-function-shield/
*
* Рассмотрим карту регистров Basic Interval Timer Register Map
* -----------------------------------------------------------------------------------
* | Name   | Direction | Default | Description                                      |
* -----------------------------------------------------------------------------------
* | BITCNT |    R      |   00H   |  Регистр счетчика базового интервального таймера |
* -----------------------------------------------------------------------------------
* | BITCR  |    R/W    |   45H   |  Регистр управления базовым интервальным таймером|
* -----------------------------------------------------------------------------------
*
*\ author ScuratovaAnna 
*\ сode debugging PivnevNikolay
*/

#include    "Intrins.h"
#include    "delay.h"     
#include "a96g166_gpio.h"
#include "a96g166_clock.h"
#include "a96g166_bit.h"

volatile u16 count;
//-----------------------------------------------------
void Delay (u16 ms){
  BIT_Initial(BIT_DIV128, BIT_X128);   
  BIT_Interrupt_config(TRUE);
  count = ms;
  while(count);
}
//-----------------------------------------------------
void BIT_Int_Handler(void) interrupt BIT_VECT
{
  if (count){count --;}		
  BIT_ClearInterruptStatus();
}
//-----------------------------------------------------
void Main(void)
{
  GLOBAL_INTERRUPT_DIS();   
  Port_Initial();	
  Clock_Initial(HSI32_DIV2);
  GLOBAL_INTERRUPT_EN();
  Port_SetOutputpin(PORT1,PIN0,PUSH_PULL);	
    while(1)
   {
      Port_SetOutputTogglepin(PORT1,PIN0);
      Delay(1000);
   }
}
//--------------End Of File----------------------------
