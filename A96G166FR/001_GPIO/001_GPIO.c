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
/*---------------------------------------------------------------------------------------------------------------------------------------
* -----------------------------------  Второй пример кнопки и светодиоды.  --------------------------------------------------------------
-------------------------------- При нажатии на одну из кнопок будет загораться один светодиод. -----------------------------------------
*     A96G166FR
*   ------------
*  |            |
*  |            |
*  |            |
*  |        P1.0| --> LED D1 P13
*  |        P1.1| --> LED D2 P12
*  |        P1.2| --> LED D3 P11
*  |        P1.3| --> LED D4 P10
*  |            |
*  |        P2.4| <-- Button A1
*  |        P2.5| <-- Button A2
*  |        P3.0| <-- Button A3
*  |            |
*  |            |
*
* Внимание стоит обратить на функцию библиотеки a96g166_gpio.с
* void Port_SetInputpin(uint8_t port, uint8_t pin, uint8_t mode)
*{  switch(port)
*  {
*   case 0:
*    P0IO &= ~(1<<pin);					//Set input mode
*    P0PU |= (mode<<pin);				//Set Pull up mode disable : 0, enable : 1
*    break;
*    case 1:
*     P1IO &= ~(1<<pin);
*     P1PU |= (mode<<pin);
*     break;
*     case 2:
*     P2IO &= ~(1<<pin);
*     P2PU |= (mode<<pin);
*     break;
*     default:
*     break;
* }
*}
*  В данной функции отсутствует порт 3 (выводы Р3.0 Р3.1 Р3.2 .... Р3.7 не могут быть настроены как входы!!!)
*  Также возможны проблемы с настройкой ваводов при использовании функции библиотеки!!! 
*/
#include "Intrins.h"
#include "delay.h"
#include "a96g166_gpio.h"
#include "a96g166_clock.h"

//bit condition;
u8 condition = TRUE;

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
  Port_SetOutputpin(PORT1, PIN0, PUSH_PULL);
  Port_SetOutputpin(PORT1, PIN1, PUSH_PULL);
  Port_SetOutputpin(PORT1, PIN2, PUSH_PULL);
  Port_SetOutputpin(PORT1, PIN3, PUSH_PULL);
  Port_SetOutputHighpin(PORT1, PIN0);
  Port_SetOutputHighpin(PORT1, PIN1);
  Port_SetOutputHighpin(PORT1, PIN2);
  Port_SetOutputHighpin(PORT1, PIN3);
  //Port_SetInputpin(PORT2, PIN4, NO_PULL);//P2.4
  P2IO &= ~(1<<4);//SetInputpin
  P2PU &= ~(1<<4);// NO_PULL
  //Port_SetInputpin(PORT2, PIN5, NO_PULL);//P2.5
  P2IO &= ~(1<<5);//SetInputpin
  P2PU &= ~(1<<5);// NO_PULL
  //Port_SetInputpin(PORT3, PIN0, NO_PULL);//P3.0
  P3IO &= ~(1<<0);//SetInputpin
  P3PU &= ~(1<<1);// NO_PULL
	
  while(1)
  {
  condition = Port_GetInputpinValue(PORT2, PIN4);
   if(condition == FALSE){
    Port_SetOutputLowpin(PORT1, PIN0);	
     }
    else 
    Port_SetOutputHighpin(PORT1, PIN0);
    condition = Port_GetInputpinValue(PORT2, PIN5);
    if(condition == FALSE){
    Port_SetOutputLowpin(PORT1, PIN1);	
    }
    else 
    Port_SetOutputHighpin(PORT1, PIN1);
    condition = Port_GetInputpinValue(PORT3, PIN0);
    if(condition == FALSE){
    Port_SetOutputLowpin(PORT1, PIN2);	
    }
    else 
    Port_SetOutputHighpin(PORT1, PIN2);
  }
}
/*---------------------------------------------------------------------------------------------------------------------------------------
* ----------------------------------- Третий пример внешние прерывания. -----------------------------------------------------------------
* Внешние прерывания - это прерывания по событиям на выводах микроконтроллера (восходящий или падающий фронты).
* Все прерывания могут быть включены либо выключены  GLOBAL_INTERRUPT_DIS();  GLOBAL_INTERRUPT_EN(); EA = 1 (включено) , EA = 0 (выключено).
* Контакты EINT0 по EINT12  принимают различные запросы на прерывания в зависимости от полярности регистра внешнего прерывания.
* Каждый внешний источник прерывания имеет бит включения и выключения.  
* Регистр флага внешнего прерывания (FLAG) предоставляет статус внешнего прерывания. 
*     A96G166FR
*   ------------
*  |            |
*  |            |
*  |            |
*  |        P1.0| --> LED D1 P13
*  |        P1.1| --> LED D2 P12
*  |        P1.2| --> LED D3 P11
*  |        P1.3| --> LED D4 P10
*  |            |
*  |        P0.3| <-- Button A1 EINT1
*  |        P2.1| <-- Button A2 EINT8
*  |        P2.2| <-- Button A3 EINT9
*  |            |
*  |            |
*
* 
*/
#include "Intrins.h"
#include "delay.h"
#include "a96g166_gpio.h"
#include "a96g166_clock.h"

//---------------------------------------------------------------
//External Interrupt Description EINT0...EINT4 --> INT5 Interrupt 
//User’s manual page 58 (страница 58)
void GPIO_Int_Handler(void) interrupt EINT04_VECT
{
  Port_SetOutputTogglepin(PORT1, PIN0);
  Port_ClearInterruptStatus(EXTINT_CH1);
}
//---------------------------------------------------------------
void Main(void)
{
  GLOBAL_INTERRUPT_DIS();       
  Port_Initial();		        
  Clock_Initial(HSI32_DIV16);//DIV16!!!        
  GLOBAL_INTERRUPT_EN(); 
  Port_SetOutputpin(PORT1, PIN0, PUSH_PULL);
  Port_SetOutputpin(PORT1, PIN1, PUSH_PULL);
  Port_SetOutputpin(PORT1, PIN2, PUSH_PULL);
  Port_SetOutputpin(PORT1, PIN3, PUSH_PULL);
  Port_SetOutputHighpin(PORT1, PIN0);
  Port_SetOutputHighpin(PORT1, PIN1);
  Port_SetOutputHighpin(PORT1, PIN2);
  Port_SetOutputHighpin(PORT1, PIN3);
  //Port_SetInputpin(PORT0, PIN3, NO_PULL);//P0.3
  P0IO &= ~(1<<3);//SetInputpin
  P0PU &= ~(1<<3);// NO_PULL
  P0DB &= ~(1<<6);//Configure De-bounce Clock of Port fx/4096
  P0DB |= (1<<1)|(1<<7);//De-bounce Enable
  //Port_SetInputpin(PORT2, PIN1, NO_PULL);//P2.1
  P2IO &= ~(1<<1);//SetInputpin
  P2PU &= ~(1<<1);// NO_PULL
  //Port_SetInputpin(PORT2, PIN2, NO_PULL);//P2.2
  P2IO &= ~(1<<2);//SetInputpin
  P2PU &= ~(1<<2);// NO_PULL	
  P12DB|= (1<<5)|(1<<6);//De-bounce Enable
	
  Port_EnableInterrupt(EXTINT_CH1);
  //Port_EnableInterrupt(EXTINT_CH8);
  //Port_EnableInterrupt(EXTINT_CH9);
  Port_ConfigureInterrupt(EXTINT_CH1, FALLING_EDGE);
	
  while(1)
  {
  
  }
}
