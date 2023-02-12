/**---------------------------------------------
*\date  09.02.2023
*\brief
*     A96G166FR
*   ------------
*  |            |
*  |            |
*  |            |
*  |        P1.0| --> LED D1 P13
*  |        P1.1| --> LED D2 P12
*  |        P1.2| --> LED D3 P11
*  |        P1.3| --> LED D4 P10
*
*\authors ScuratovaAnna 
*
* ARDUINO MULTI-FUNCTION SHIELD
* Идея и примеры взяты отсюда  http://publicatorbar.ru/2017/12/21/arduino-multi-function-shield/
* Все примеры реализованы с применением ARDUINO MULTI-FUNCTION SHIELD
* Рассмотрим карту регистров GPIO для порта Р1 Port
* -----------------------------------------------------------
* |P1    |  R/W  |   0x00   |P1 Data Register               |  выход --> данные могут быть записыны в этот регистр 
* |      |       |          |                               |  вход  --> 
* -----------------------------------------------------------
* |P1IO  |  R/W  |   0x00   |P1 Direction Register          |  Каждый вывод ввода-вывода может независимо использоваться в качестве входа или выхода 
* |      |       |          |                               |  Регистр конфигурации  0 --> Input   |    1 --> Output
* -----------------------------------------------------------
* |P1PU  |  R/W  |   0x00   |P1 Pull-up register selection  |  Встроенный подтягивающий резистор может быть подключен к портам ввода-вывода 
* |      |       |          |                               |  индивидуально с помощью регистра выбора подтягивающего резистора 
* |      |       |          |                               |  Configure Pull-up Resistor  0 --> Disable  1 --> Enable
* -----------------------------------------------------------
* |P1OD  |  R/W  |   0x00   |P1 Open-drain Selection Reg    |  Configure Open-drain of P1 Port
* |      |       |          |                               |  0 --> Push-pull output
* |      |       |          |                               |  1 --> Open-drain output
* -----------------------------------------------------------
* Регистр P1DB (P1 Debounce Enable Register) будет описан в примере с кнопками , функция может быть задействована только
* для выводов порта P0(P0DB) и  выводов порта P1 (P1OD) , номера выводов портов будут указаны в таблице отдельно.
* P1FSRH и P1FSRL эти регистры определяют альтернативные функции портов. 
* Так же линии ввода вывода помимо конфигурации как аналоговый вход , могут быть сконфигурированы как входы для внешних прерываний.
*******************************************************************************************************************************************
*\brief
*/
/*---------------------------------------------------------------------------------------------------------------------------------------
* --------------------------------  Первый пример поморгаем светодиодами.  --------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------------------------*/
#include "Intrins.h"
#include "delay.h"
#include "a96g166_gpio.h"
#include "a96g166_clock.h"

u8 CW[4]={0xE,0xD,0xB,0x7};
u8 i;

void Main(void)
{
  // Disable INT. during peripheral setting
  GLOBAL_INTERRUPT_DIS();       
  // Port intialize 
  Port_Initial();       
  // Clock 16Mhz initialize 
  Clock_Initial(HSI32_DIV2);         
  // Enable INT 
  GLOBAL_INTERRUPT_EN(); 
  //Port_SetOutputpin(PORT1, PIN0|PIN1|PIN2|PIN3, PUSH_PULL); --> данная инициализация не корректна и в дальнейшем использоваться не будет!
  Port_SetOutputpin(PORT1, PIN0, PUSH_PULL);
  Port_SetOutputpin(PORT1, PIN1, PUSH_PULL);
  Port_SetOutputpin(PORT1, PIN2, PUSH_PULL);
  Port_SetOutputpin(PORT1, PIN3, PUSH_PULL);
  /*------------------------------------------------
  P1IO |= (1<<0)|(1<<1)|(1<<2)|(1<<3);
  P1OD &= ~((1<<0)|(1<<1)|(1<<2)|(1<<3));
  ------------------------------------------------*/
  //Port_SetOutputHighpin(PORT1, PIN0|PIN1|PIN2|PIN3);--> данная инициализация не корректна и в дальнейшем использоваться не будет!
  Port_SetOutputHighpin(PORT1, PIN0);//логическая единица на выходе P1.0 
  Port_SetOutputHighpin(PORT1, PIN1);//логическая единица на выходе P1.1
  Port_SetOutputHighpin(PORT1, PIN2);//логическая единица на выходе P1.2 
  Port_SetOutputHighpin(PORT1, PIN3);//логическая единица на выходе P1.3 
  /*------------------------------------------------
  P1 |= (1<<0)|(1<<1)|(1<<2)|(1<<3);
  ------------------------------------------------*/
  
  while(1)
  {
    for (i=0;i<4;i++){
    P1=*(CW+i);
    Delay_ms(200);
  }
 }
}
//----------------------------------------------------------------------------------------------------------------------------------------//