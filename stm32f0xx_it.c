// ******************************************************************************
// ********    Plik obsługi przerwań STM32F0                 ********************
// ******************************************************************************


// Includes ------------------------------------------------------------------
//#include "stm32f0xx_it.h"
#include "stm32f0xx.h"
//#include "gpio_def.h"
//#include <stdio.h>


/*
extern volatile uint8_t f_systick;
extern uint8_t f_tim15;
extern uint8_t buf_rx2[20];
extern uint8_t wsk_rx2;
extern uint8_t f_rx2;






extern volatile uint16_t timer1;
extern volatile uint16_t timer2;
extern volatile uint16_t timer3;
extern volatile uint32_t timer4;

extern volatile uint16_t timer5;  //cyklicznie ustawia flage f_tim5
extern volatile uint8_t f_tim5;

extern volatile uint16_t timer6;  //
extern volatile uint8_t f_tim6;
*/





/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/


void NMI_Handler(void)
{
}


void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}


void SVC_Handler(void)
{
}


void PendSV_Handler(void)
{
}

//============================================================================
void SysTick_Handler(void)   //T=10ms      //??? 50ms-> 20x/sek
{  		
	if(timer1)  { timer1--;   }
	if(timer2)  { timer2--;   }
	
	if(timerBlink)  timerBlink--;
	if(timerBlinkHoldOff) timerBlinkHoldOff--;
	if(timerNoAction) timerNoAction--;
	if(timerAlarm) timerAlarm--;
	
	if(timKl_imp && !kl_imp) timKl_imp--;
	if(timKl_up && !kl_up) timKl_up--;
	if(timKl_down && !kl_down) timKl_down--;
	if(timKl_left && !kl_left) timKl_left--;
	
	timeProgram++;
	
	//f_systick=1;	
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/


void USART1_IRQHandler(void)   //obsluga RS z Hytery do Radia
{
	//uint8_t x;
	//x=USART1->RDR;       
    
}

//--------------------------------------------------------
void USART2_IRQHandler(void)          //USART2 - Bluetooth Lub RS485
{  uint8_t x;
  x=USART2->RDR;       
    
	if((wsk_rx2>35) || (x==':'))       //zabezpieczenie przed przepelnieniem buf
		{  wsk_rx2=0;                         //: - pocz ramki
    }
  buf_rx2[wsk_rx2++]=x;
		
	if(x==10)                 // koniec paczki bajtow
	    {
		   f_rx2=wsk_rx2-3;  //flaga jest zarazem informacją o dlugosci pakietu
       wsk_rx2=0;   //jak odbierze cmd wsk na pocz buf_rx1
	    }
}


//==================================================================================
void EXTI2_3_IRQHandler(void)
{
  if((EXTI->PR & PB2) != 0)        // sprawdz linie 2
     {
      EXTI->PR |= PB2;        //skasuj flage przerwania od linii 2
     }
}
//=========================================================================
void TIM15_IRQHandler(void)
{
  if(TIM15->SR & TIM_SR_UIF)
    {
     TIM15->SR = (uint16_t) ~TIM_SR_UIF;
    }

}

//=======================================
void TIM14_IRQHandler(void)
{
  if(TIM14->SR & TIM_SR_UIF)
    {
     TIM14->SR = (uint16_t) ~TIM_SR_UIF;
		 
   }
 }

//================================================================
/*void TIM6_DAC_IRQHandler(void)   //nazwa TIM6_IRQHandler  nie działa
{
	//volatile uint8_t i=0;
	
     TIM6->SR = (uint16_t) ~TIM_SR_UIF;   
}  */


//==============================================================================
void TIM3_IRQHandler(void)
{
  if(TIM3->SR & TIM_SR_UIF)
    {
     TIM3->SR = (uint16_t) ~TIM_SR_UIF;
    }
		
  if(TIM3->SR & TIM_SR_CC3IF)           //przerw.od porówn CC3
    {
     TIM3->SR = (uint16_t) ~TIM_SR_CC3IF;
    }
		
  if(TIM3->SR & TIM_SR_CC4IF)          //przerw.od porówn CC4
    {	
     TIM3->SR = (uint16_t) ~TIM_SR_CC4IF;
    }		
}

//==============================================================================



//==============================================================================


//===================================================================================

//----------------------------------------

//----------------------------------------
