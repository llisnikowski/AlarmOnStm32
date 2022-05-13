/// *********************************************************
/// ***   PWSZ  EiTM AiR                 15.03.2021      ****
/// ***   Nucleo STM32F072              Racibórz 2021    ****
/// ***   Płytki dydaktyczne dla studentów               ****
/// ***   kształcenie zdalne                             ****
/// *********************************************************

//=================================================================
//==============* Dołączane biblioteki        *====================
//=================================================================
#include "stm32f0xx.h"  //definicje rejestrów - \Libraries\CMSIS
#include <stdio.h>
#include <stdlib.h>
#include "gpio_init_f0.c"
#include <stdbool.h>	//dodaje zmienne typu bool

#include "rtc.h"
#include "clock.h"


//=================================================================
//==============* Prywatne definicje         *====================
//=================================================================
//------------------------potrzebne tylko dla printf
#define PUTCHAR_PROTOTYPE int sendchar(int ch, FILE *f)

#define kl_up    (!(GPIOA->IDR&PA15))   //kl_up
#define kl_down  (!(GPIOF->IDR&PF1))    
#define kl_left  (!(GPIOC->IDR&PC12))   

#define kl_right (!(GPIOC->IDR&PC13))   //PC13 zwiera do masy
#define kl_imp   (!(GPIOC->IDR&PC13))   //PC13 zwiera do masy
#define kl_blue  (!(GPIOC->IDR&PC13))   //PC13 zwiera do masy


//=================================================================
//==============* Prywatne zmienne globalne   *====================
//=================================================================


uint16_t j;
uint32_t t;
//char a=0;
volatile uint8_t led,bar;
uint8_t dir_led=1, dir_bar=1;
volatile int8_t impuls;

uint8_t buf_rx1[32];
uint8_t wsk_rx1=0;
uint8_t f_rx1=0;

uint8_t buf_rx2[32];
uint8_t wsk_rx2=0;
uint8_t f_rx2=0;

uint8_t buf_i2c[32];   //bufor zapisu/odczytu i2c

uint16_t timer1;
uint16_t timer2;


volatile uint8_t timKl_imp;
volatile uint8_t timKl_up;
volatile uint8_t timKl_down;
volatile uint8_t timKl_left;

volatile uint32_t timeProgram;

#define TIME_ALARM_SHIFT_REG 5
volatile uint16_t timerAlarm;  


//======================================================
//===========*  Prywatne prototypy funkcji    *=========
//======================================================
void waitus(uint32_t  us);
void RCC_init(void);
void alarmMode(void);


//=================================================================
//=========*    Prywatne pliki biblioteczne   *====================
//=================================================================
#include "stm32f0xx_it.c"
#include "board_init.c"
#include "board_lib.c"
#include "lcd_stm0.c"
//#include "led.c"

/// *********************************************************
/// *********************************************************
/// ****      M  A  I  N            *************************
/// *********************************************************
/// *********************************************************


int main(void)
{

//-------Inicjalizacja układów peryferyjnych----------------=
GPIO_init();			//inicjalizacja wejść,wyjść
RCC_init();				//inicjalizacja RCC
Lcd_init();				//inicjalizacja wyświetlacza lcd
GPIOB->BSRR=PB7;		//włączenie podświetlenia
USART2_init(115200);  //rs232-PC

TIM3_init(64);        //Enkoder  pc67 //Inicjalizacja timera3 jako enkoder
TIM3->CNT = 32;				//Wpisanie wartości 32 do licznika enkodera
impuls=(TIM3->CNT)>>1;		//wpisanie do zmiennej impuls aktualnego stanu licznika enkodera
//I2C1_init(4);   //1-100kHz  4-400kHz
	
RTC_init();			//inicjalizacja zegara czasu rzeczywistego
RTC_alarm_init();	//inicjalizacj alarmu i przerwania zegara
//ADC1_init();
//RCC_init();

SysTick_Config(60000);      //inicjalizacja timera systemowego
SysTick->CTRL  &= ~SysTick_CTRL_CLKSOURCE_Msk ;  // to dzieli HCLK/8 dla systicka //włączenie pleskalera na 8



//wyświetlenie imion i nazwisk
Lcd_cmd(line1);  Lcd_str("RTC  Tomasz Stus");
Lcd_cmd(line2);  Lcd_str("  \x08. Li\x09nikowski");

//wykonanie sekwencji zapalenia diod
for(uint8_t i=0, k=1; i<9;i++)
{
	wr595(k); waitus(150000);
	k=k<<1;
}

//timerBlink = 0;
//timerBlinkHoldOff=0;

while(1)
{
//==============================================================

if(f_rtc){ //co 1s
		updateDateTime();
		displayClock();
		f_rtc=0;
		timerBlink = BLINK_DELAY_TURN_OFF;
		if(!timerBlinkHoldOff && settingTypeTime == tT_date)correctDay();
		if(chechAlarm() && !clockMode) alarmMode();
}else if(clockMode && !timerBlink && !timerBlinkHoldOff){
		if(clockMode==1){
			displayClock1(settingTypeTime);
		}
		else{
			displayClock2(settingTypeTime);
		}
		timerBlink = 1000;
}

if(clockMode>=1 && !timerNoAction){ setClockMode(0);f_rtc=1;}


if((TIM3->CNT>>1) != (uint16_t) impuls)
{
	//waitus(10000);
	if(clockMode==1) nextSettingTypeTime(TIM3->CR1&TIM_CR1_DIR);
	else if(clockMode==2) addValue( (TIM3->CR1 & TIM_CR1_DIR)? -1 : 1 );
	impuls = (TIM3->CNT)>>1;
}
	
if(!timKl_up && kl_up){
	timKl_up = 10;
	if(clockMode==1) nextSettingTypeTime(false);
	else if(clockMode==2) addValue(1);
}
if(!timKl_down && kl_down){
	timKl_down = 10;
	if(clockMode==1) nextSettingTypeTime(true);
	else if(clockMode==2) addValue(-1);
}

if(!timKl_left && kl_left){
	timKl_left = 10;
	if(clockMode==1) setClockMode(0);
	else if(clockMode==2) nextSettingIndex(true);
}
	
if(!timKl_imp && kl_imp){
	timKl_imp = 10;
	if(!clockMode) setClockMode(1);
	else if(clockMode==1) setClockMode(2);
	else nextSettingIndex(false);
}
	

//==============================================================
} }  // end while(1) , end main
/// ****    Koniec głównej pętli     ****************
/// *************************************************
/// *********************************************

     


void alarmMode(void){
	uint8_t sRegL=0,sRegR=0,limit=8,sRegDirect=0;
	GPIOB->BRR=PB7;	
	while(1){
		if(f_rtc){ //co 1s
			updateDateTime();
			displayClock();
			GPIOB->BSRR=PB7;
			f_rtc=0;
			timerBlink = BLINK_DELAY_TURN_OFF;
		}
		else if(!timerBlink){
			GPIOB->BRR=PB7;
			timerBlink = 1000;
		}
		
		if(!timerAlarm){
			timerAlarm = TIME_ALARM_SHIFT_REG;
			
			if(!sRegDirect){
				wr595(sRegL|(1<<sRegR++));
				if(sRegR==limit){
					sRegL = (sRegL>>1) |0x80;
					sRegR=0;
					limit--;
					printf("%d\n",sRegL);
					if(limit==0){
						limit=8;
						sRegDirect=1;
					}
				}
			}
			else{
				wr595(sRegL&~(1<<sRegR++));
				if(sRegR==limit){
					sRegL = (sRegL>>1);
					sRegR=0;
					limit--;
					printf("%d\n",sRegL);
					if(limit==0){
						limit=8;
						sRegDirect=0;
					}
				}
			}			
		}
		
		

		if((TIM3->CNT>>1) != (uint16_t) impuls)
		{
			impuls = (TIM3->CNT)>>1;
			break;
		}
	
		if(!timKl_up && kl_up){
			timKl_up = 10;
			break;
		}
		if(!timKl_down && kl_down){
			timKl_down = 10;
			break;
		}

		if(!timKl_left && kl_left){
			timKl_left = 10;
			break;
		}
	
		if(!timKl_imp && kl_imp){
			timKl_imp = 10;
			break;
		}
	}
	GPIOB->BSRR=PB7;
	wr595(0);
}




















// ============================================================
PUTCHAR_PROTOTYPE  //potrzebne dla printf
{
   USART2->TDR = (uint8_t) ch;
   while((USART2->ISR & USART_ISR_TXE)==0){};
   return ch;
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void RCC_init(void)
{
RCC->CR |= RCC_CR_CSSON | RCC_CR_HSEBYP;     //wlaczenie  HSEBYP csson 
RCC->CR|=RCC_CR_HSEON;
while((RCC->CR & RCC_CR_HSERDY) == 0) { }

FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;
RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLMUL6 );

RCC->CR |= RCC_CR_PLLON;                      //PLLON
while((RCC->CR & RCC_CR_PLLRDY) == 0) { }   // czakaj aż stabilny sygnał

RCC->CFGR |= RCC_CFGR_SW_PLL;     // Select PLL as system clock source 
while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL)  {  }
}

//------------------------------------------------------------


//------------------------------------------------------------

//*************************************************************


/// ****************END OF FILE******************************
