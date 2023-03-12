/**---------------------------------------------
*\date  09.03.2023
*\brief
* Clock generator
* Система тактирования --> в составе микроконтроллера A96G166FR есть возможность 
* переключения (выбора) источника тактового сигнала.
* Источников сигнала четыре это:
* Внутренние источники тактирования
* Internal RC oscillator (32MHZ)
* Internal LSI oscillator (128kHz)
* Внешние источники тактирования
* Main crystal oscillator (0.4~12 MHz)
* Sub-crystal oscillator (32.768 kHz)   -->  в данном микроконтроллере отсутствуют выводы для установки 
*                                             внешнего кварца частотой 32,768 KHz
********************************************************************************************************
* В данном примере по нажатию кнопки переключаем частоту внутреннего кварцевого резонатора,
* а также переходим на внешний источник тактирования 12MHz.
* Переходы от одной частоты тактирования к другой частоте тактирования можно увидеть по миганию светодиодов.
********************************************************************************************************
*     A96G166FR
*   ------------
*  |        P3.7| --> XOUT
*  |        P3.6| <-- XIN
*  |            |
*  |        P1.0| --> LED D1 P13
*  |        P1.1| --> LED D2 P12
*  |        P1.2| --> LED D3 P11
*  |        P1.3| --> LED D4 P10
*  |            |
*  |        P2.1| <-- Button A1
*  |        P2.2| <-- Button A2
*  |            |
*  |            |
* ARDUINO MULTI-FUNCTION SHIELD http://publicatorbar.ru/2017/12/21/arduino-multi-function-shield/
*
* Рассмотрим карту регистров Clock Generator Register Map
* -------------------------------------------------------------------------
* | Name | Direction | Default | Description                              |
* -------------------------------------------------------------------------
* | OCCR |    R/W    |   00H   | регистр выбора источника тактирования    |
* -------------------------------------------------------------------------
* | OSCCR|    R/W    |   28H   | регистр настройки источника тактирования |
* -------------------------------------------------------------------------
* |XTFLSR|    R/W    |   00H   | настройка и выбор фильтров при переходе  |
* |      |           |         | на внешний источник тактирования         |
* -------------------------------------------------------------------------
*\brief
*\ author ScuratovaAnna 
*\ сode debugging PivnevNikolay
*/



#include "Intrins.h"
#include "delay.h"
#include "a96g166_gpio.h"
#include "a96g166_clock.h"

bit condition = TRUE;//1
u8 Count =0;
u8 CW[4]={0xE,0xD,0xB,0x7};
u8 i;


void GPIO_Int_Handler(void) interrupt EINT7A_VECT
{
  if(!Port_GetInputpinValue(PORT2, PIN1)){//!<--
    condition = FALSE;
    Count++;
    if(Count>=6){Count=6;}
   }
  else if(!Port_GetInputpinValue(PORT2, PIN2)){//!<--
    condition = FALSE;
    if(Count==0){Count=1;}
    Count--;
   }
   Port_ClearInterruptStatus(EXTINT_CH8);
   Port_ClearInterruptStatus(EXTINT_CH9);
}

void Main(void)
{
  GLOBAL_INTERRUPT_DIS();       
  Port_Initial();       
  Clock_Initial(HSI32_DIV2);// 16 MHz	
  GLOBAL_INTERRUPT_EN(); 

  //Port_SetOutputpin(PORT1, PIN0|PIN1|PIN2|PIN3, PUSH_PULL);
  Port_SetOutputpin(PORT1, PIN0, PUSH_PULL);
  Port_SetOutputpin(PORT1, PIN1, PUSH_PULL);
  Port_SetOutputpin(PORT1, PIN2, PUSH_PULL);
  Port_SetOutputpin(PORT1, PIN3, PUSH_PULL);
  /*------------------------------------------------
  P1IO |= (1<<0)|(1<<1)|(1<<2)|(1<<3);
  P1OD &= ~((1<<0)|(1<<1)|(1<<2)|(1<<3));
  ------------------------------------------------*/

  //Port_SetOutputHighpin(PORT1, PIN0|PIN1|PIN2|PIN3);
  Port_SetOutputHighpin(PORT1, PIN0);
  Port_SetOutputHighpin(PORT1, PIN1);
  Port_SetOutputHighpin(PORT1, PIN2); 
  Port_SetOutputHighpin(PORT1, PIN3);
  /*------------------------------------------------
  P1 |= (1<<0)|(1<<1)|(1<<2)|(1<<3);
  ------------------------------------------------*/

  //Port_SetInputpin(PORT2, PIN1, NO_PULL);//P2.1
  P2IO &= ~(1<<1);//SetInputpin
  P2PU &= ~(1<<1);// NO_PULL
  //Port_SetInputpin(PORT2, PIN2, NO_PULL);//P2.2
  P2IO &= ~(1<<2);//SetInputpin
  P2PU &= ~(1<<2);// NO_PULL
  P12DB|= (1<<5)|(1<<6);

  Port_EnableInterrupt(EXTINT_CH8);
  Port_ConfigureInterrupt(EXTINT_CH8, FALLING_EDGE);
  Port_EnableInterrupt(EXTINT_CH9);
  Port_ConfigureInterrupt(EXTINT_CH9, FALLING_EDGE);

    while(1)
  {
  switch(Count){
  case 0:
   SCCR	= 0x0;  // внутренний высокоскоростной источник тактирования
   OSCCR = 0x28;// 16Mhz
   condition = TRUE;
  break;
  case 1:
   SCCR	= 0x0;  // внутренний высокоскоростной источник тактирования
   OSCCR = 0x20;// 8Mhz
   condition = TRUE;
  break;
  case 2:
   SCCR	= 0x0;  // внутренний высокоскоростной источник тактирования
   OSCCR = 0x18;// 4MHz
   condition = TRUE;
  break;
  case 3:
   SCCR	= 0x0;  // внутренний высокоскоростной источник тактирования
   OSCCR = 0x10;// 2MHz
   condition = TRUE;
  break;
  case 4:
   SCCR	= 0x0;  // внутренний высокоскоростной источник тактирования
   OSCCR = 0x8; // 1Mhz
   condition = TRUE;
  break;
  case 5:
   SCCR	= 0x0;  // внутренний высокоскоростной источник тактирования
   OSCCR = 0x0; // 0.5MHz
   P3FSRH = 0x0;
   condition = TRUE;
  break;
  case 6:
   P3FSRH |= (1 << 2) | (1 << 1); // Настраиваем выводы P3.7(XOUT) P3.6(XIN) Для работы с внешним кварцевым резонатором (12MHz)
   OSCCR |= (1 << 1);//HSE(External clock) operation enable
   Delay_ms(1);//stabilization time.
   SCCR	= 0x1;// Включаем работу внешнего кварцевого резонатора (12MHz)
   condition = TRUE;
  break;
  }
  while(condition)
  {
    //мигаем светодиодами
    for (i=0;i<4;i++){
    P1=*(CW+i);
    Delay_ms(10);
   }
  }
 }
}