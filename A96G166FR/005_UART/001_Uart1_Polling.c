/**---------------------------------------------
*\date  04.04.2024
*\brief
* Uart1 Polling
* Универсальный синхронный и асинхронный последовательный приемник и передатчик (USART) - это
* очень гибкое устройство последовательной связи. A96G166 имеет два функциональных 
* блока USART --> USART0 и USART1 абсолютно идентичны по функционалу.
*
*************************************************************************
* В данном примере мигаем светодиодом и посылаем статус светодиода в UART1.
*************************************************************************
*
*     A96G166FR
*   ------------
*  |            |
*  |        P1.0| --> LED 
*  |            |
*  |        P2.4| --> UART_TX
*  |        P2.5| <-- UART_RX
*
* \authors ScuratovaAnna 
* \сode debugging PivnevNikolay
************************************************************************
*\brief
*/
/* Includes ----------------------------------------------------------*/
#include "compiler.h"
#include "delay.h"
#include "a96g166_gpio.h"
#include "a96g166_clock.h"
#include "a96g166_usart1.h"

bit i;

void putstring(uint8_t *str);
void Set_Bit(void);
void Clr_Bit(void);

void main( void )
{
	//GLOBAL_INTERRUPT_DIS();
	EA = 0;
  /* Port initialize */
	Port_Initial();		        
    /* Clock initialize */
	Clock_Initial(HSI32_DIV2);         
    /* System Stabilize Delay */
	Delay_ms(60);
	
	SET_BIT(P1IO,PIN0);
	CLR_BIT(P1OD,PIN0);
    //Port_SetOutputpin(PORT1, PIN0, PUSH_PULL); --> P1.0
	SET_BIT(P1,PIN0);
    //Port_SetOutputHighpin(PORT1, PIN0); --> P1.0 
/**********************************************************************/
	//Set Alernative Function for USART P25(RX) / P24(TX)
	//Port_SetAlterFunctionpin(PORT2, PIN4, 0x1);
	//Port_SetAlterFunctionpin(PORT2, PIN5, 0x1);
	P2FSRH = 0x05;
	P2FSRL = 0x00;
/**********************************************************************/
	USART1_Initial(9600, USART1_DATA_8BIT, USART1_STOP_1BIT, USART1_PARITY_NO, USART1_TX_RX_MODE);	
	//GLOBAL_INTERRUPT_EN();
  EA = 1;
	
	putstring("a96g166 USART POLLING TEST\n\r");	
	Delay_ms(50);

	while (1)
	{
	  //USART1_ReceiveDataWithPolling(&temp, 1);
	  //USART1_SendDataWithPolling(&temp, 1);
	  (i == FALSE)?(Set_Bit()):(Clr_Bit());
	   i = !i;
	  Delay_ms(1000);
	}
}
/**********************************************************************/
 void putstring(uint8_t *str)
{
	while(*str != 0)
		USART1_SendDataWithPolling(str++, 1);
}
/**********************************************************************/
 void Set_Bit(void){
	SET_BIT(P1,PIN0);
	putstring("SET BIT P1.0\n\r");	
}
/**********************************************************************/
 void Clr_Bit(void){
	CLR_BIT(P1,PIN0);
	putstring("CLR BIT P1.0\n\r");	
}
/* --------------------------- End Of File -------------------------- */