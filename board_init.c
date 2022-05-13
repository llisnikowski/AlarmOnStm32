/// *********************************************************
/// ******  GPIO Setup                            ***********
/// *********************************************************
//zasada: tu inicjuje się piny na stałe przypisane do płytki
//oraz wszystkie peryferia na stałe przypisane do płytki
//ulozone wg układów peryferyjnych

#include "stm32f0xx.h"  //definicje rejestrów - \Libraries\CMSIS
#include "lcd_stm0.h"

//============================================================================

void GPIO_init(void)
{

RCC->AHBENR |= RCC_AHBENR_GPIOAEN|RCC_AHBENR_GPIOBEN|RCC_AHBENR_GPIOCEN|RCC_AHBENR_GPIODEN|RCC_AHBENR_GPIOFEN;

//=====================================================================
//=======     GPIO LCD

gpio_pin_cfg(GPIOC, PC0, OUT_PP_HS_No);    //0,1,2,3->lcd D4..D7;
gpio_pin_cfg(GPIOC, PC1, OUT_PP_HS_No);
gpio_pin_cfg(GPIOC, PC2, OUT_PP_HS_No);  //WE 595
gpio_pin_cfg(GPIOC, PC3, OUT_PP_HS_No);  //shcp 595
	
gpio_pin_cfg(GPIOC, PC11, OUT_PP_HS_No);   //lcd_rs
gpio_pin_cfg(GPIOD, PD2,  OUT_PP_HS_No);    //pd2:lcd_e
	
gpio_pin_cfg(GPIOB, PB7, OUT_PP_LS_No);   //lcd_podświetlenie

//74HC595	
gpio_pin_cfg(GPIOC, PC10, OUT_PP_HS_No);  //STCP 595
	
//--------------------------------------------------------------------
//---------  Przyciski klawiszy

gpio_pin_cfg(GPIOC, PC13, IN_PU );   //klawisz user niebieski i kl_imp
gpio_pin_cfg(GPIOA, PA15, IN_PU );   //kl_up
gpio_pin_cfg(GPIOC, PC12, IN_PU );   //kl_left
gpio_pin_cfg(GPIOF, PF1,  IN_PU );   //kl_down
	
//---------------------------------------------------------------------
//------  wejścia analogowe

gpio_pin_cfg(GPIOB, PB0,IN_Analog);  //ADC_IN8 fotorezystor

//--------------------------------------------------------------------
//  pozostałe
  gpio_pin_cfg(GPIOA, PA5, OUT_PP_HS_No);   //green led

}  //end gpio init
//================================================================
//================   ADC1 Configuration    =======================
void ADC1_init(void)
{
RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; //włącz taktowanie ADC
//ADC1->CFGR2=0x00000000; //taktowanie z HSI14 RCC_CR2
ADC1->CFGR2=0x80000000; //taktowanie z PCLK/4 -> 12MHZ
ADC1->SMPR=3; //próbkowanie 3=28.5 cykli 2=13.5
ADC1->CR=ADC_CR_ADCAL; //włącz kalibrację
while((ADC1->CR & ADC_CR_ADCAL)==ADC_CR_ADCAL); //kasowany sprzętowo
//po kalibracji
ADC1->DR; //musi być odczyt lub odczekać czas
ADC1->CR=ADC_CR_ADEN; //włącz ADC
while((ADC1->ISR&ADC_ISR_ADRDY )!=1); //czekaj na flagę ADCEN
}

//===========================================================================
//            ---   I2C1   ---
//===========================================================================
// hi2c1.Init.Timing = 0x20303E5D;  wg cube to jest 100kHz
//                     0x2010091A                   400kHz
void I2C1_init(uint8_t speed)
{
gpio_pin_cfg(GPIOB, PB8, AF_OD_HS_Up);    //PB8: I2C1_SCL
gpio_pin_cfg(GPIOB, PB9, AF_OD_HS_Up);    //PB9: I2C1_SDA
gpio_af_cfg(GPIOB, PB8, 1);              //AF1 I2C1_SCL
gpio_af_cfg(GPIOB, PB9, 1);              //AF1 I2C1_SDA
GPIOB->BSRR= PB8 | PB9;                   //  ustaw na 1

RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
  
if(speed==4) I2C1->TIMINGR = (uint32_t)0x2010091A;    //400kHz   /
else         I2C1->TIMINGR = (uint32_t)0x20303E5D;    //100kHz   //
I2C1->CR1 = I2C_CR1_PE;
}

//===========================================================================
//=======     Init   Timers                      ============================
//===========================================================================
void TIM3_init(uint16_t arr)
{
RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;   // TIM3 clock enable

gpio_pin_cfg(GPIOC, PC6, AF_PP_HS_Up);
gpio_pin_cfg(GPIOC, PC7, AF_PP_HS_Up);

TIM3->SMCR = TIM_SMCR_SMS_1;   //tryb encoder nr2 ; dodanie | TIM_SMCR_SMS_0 zlicza 4 zbocza
TIM3->CCMR1 = TIM_CCMR1_IC1F | TIM_CCMR1_IC2F;   //włącz filtry wejściowe //też działa bez
TIM3->CCER = TIM_CCER_CC1P;       //polaryzacja we \_; bez tego liczy w druga strone
TIM3->ARR = arr;                 //rej repetycji
// TIM3->DIER = TIM_DIER_UIE | TIM_DIER_CC3IE; // | TIM_DIER_CC4IE;        //wlacz przerwania od przepełnienia i ccr3
TIM3->CR1 = TIM_CR1_CEN;          //wlacz licznik

// NVIC_SetPriority(TIM3_IRQn, 0);
// NVIC_ClearPendingIRQ(TIM3_IRQn);
// NVIC_EnableIRQ(TIM3_IRQn);
}
//====================================================================================
//tim2  może pracować jako 32-bit enloder PB3-ch2 i PA15-ch1  (bez karty uSC i nRF)
//tim2  ch 3 i 4 jako pwm dla serva
__INLINE void TIM2_init(void)
{
  gpio_pin_cfg(GPIOA, PA15, AF_PP_HS_No);   //ch1
  gpio_pin_cfg(GPIOB, PB3, AF_PP_HS_No);   //ch2
  gpio_af_cfg(GPIOA, PA15, 2);              //AF2 tim2_ch1
  gpio_af_cfg(GPIOB, PB3, 2);              //AF2 tim2_ch2

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;  //taktowanie TIM2

  TIM2->SMCR = TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0;   //tryb encoder nr2 ; dodanie | TIM_SMCR_SMS_0 zlicza 4 zbocza
  TIM2->CCMR1 = TIM_CCMR1_IC1F | TIM_CCMR1_IC2F;   //włącz filtry wejściowe //też działa bez
  //TIM2->CCER = TIM_CCER_CC1P;       //polaryzacja we \_; bez tego liczy w druga strone
  TIM2->ARR = 65535;                 //rej repetycji
 // TIM2->DIER = TIM_DIER_UIE | TIM_DIER_CC3IE; // | TIM_DIER_CC4IE;        //wlacz przerwania od przepełnienia i ccr3

  TIM2->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;   //arpe-buforowanie
}

//================================================================
//   wy - PB8
__INLINE void TIM16_init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;  //taktowanie TIM16

    gpio_pin_cfg(GPIOB, PB8, AF_PP_HS_No);
    gpio_af_cfg(GPIOB, PB8, 2);

		TIM16->PSC = 470;              //preskaler f=1kHz
		TIM16->ARR = 1023;              //PWM 10-bitowy
		TIM16->CCMR1 = TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;  //pwm 110,

		TIM16->CCER =	TIM_CCER_CC1E;      //włączenie wyjścia output enable
		//TIM16->EGR = TIM_EGR_UG;
		TIM16->CCR1 = 0;               //wartość do porównania z CNT
		TIM16->BDTR = TIM_BDTR_MOE;
		TIM16->CR1 =  TIM_CR1_CEN;  //buforowanie przeładowania TIM_CR1_ARPE | i załączenie

}

///================================================================
//   wy - PB9
__INLINE void TIM17_init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;  //taktowanie TIM17

    gpio_pin_cfg(GPIOB, PB9, AF_PP_HS_No);
    gpio_af_cfg(GPIOB, PB9, 2);
	

		TIM17->PSC = 470;              //preskaler f=1kHz
		TIM17->ARR = 511;               //PWM 10-bitowy
		TIM17->CCMR1 = TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;  //pwm 110,

		TIM17->CCER =	TIM_CCER_CC1E;      //włączenie wyjścia output enable
		//TIM17->EGR = TIM_EGR_UG;
		TIM17->CCR1 = 0;               //wartość do porównania z CNT
		TIM17->BDTR = TIM_BDTR_MOE;
		TIM17->CR1 =  TIM_CR1_CEN;  //buforowanie przeładowania TIM_CR1_ARPE | i załączenie

}
//***************************************************************


//-------------------------------
void USART2_init(uint32_t baud)    //pa2 i pa3
{
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

  gpio_pin_cfg(GPIOA, PA2, AF_PP_HS_Up);  //Tx PA2
  gpio_pin_cfg(GPIOA, PA3, AF_PP_HS_Up);  //Rx PA3
  gpio_af_cfg(GPIOA, PA2, 1);
  gpio_af_cfg(GPIOA, PA3, 1);

  USART2->BRR = 48000000/baud;
  USART2->CR1 = USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;
    //            wl. usart      wl.przerw.od Rx    TX Enable       RX enable

  NVIC_EnableIRQ(USART2_IRQn);   //ta funkcja jest w core_cm0.h
}

//=================================================================================
// ********************************************************************************


//-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --


// ********************************************************************************

//======================================================================
//Licznik dla przerwań/triger DAC
__INLINE void TIM6_init(void)   //na razie przerwanie
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;  //taktowanie TIM6

	TIM6->PSC = 0;         //preskaler   f= 1/psc+1  48MHz/240=200kHz
  TIM6->ARR = 187;       //autoreload   tez +1      200kHz/4000=50Hz

//	TIM6->DIER = TIM_DIER_UIE;   //wł przerwania
 	//TIM6->EGR = TIM_EGR_UG;  //wł event gener. -wtedy  przeładuje nową wartość po buforowaniu
  TIM6->CR2 = TIM_CR2_MMS_1;   //010
  TIM6->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;   //arpe-buforowanie   TIM_CR1_ARPE |

	//NVIC_EnableIRQ(TIM6_DAC_IRQn);    // włącz NVIC dla tim6_DAC
}


//====================================================================================

void EXTI_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	gpio_pin_cfg(GPIOF, PF5, IN_PD);

  EXTI->IMR = 0x0005 |PC8 | PF5;  //PA0 |PB2;  //0x0001;      // PA0
  //EXTI->RTSR = PF5;     //rising edge
  EXTI->FTSR = 0x0005 |PC8 |PF5;   //falling edge

	SYSCFG->EXTICR[0]  |= SYSCFG_EXTICR1_EXTI2_PB;     //linie 0-3
	SYSCFG->EXTICR[1]  |= SYSCFG_EXTICR2_EXTI5_PF;     //linie 4-7
  SYSCFG->EXTICR[2]  |= SYSCFG_EXTICR3_EXTI8_PC;     //linie 8-11
	//SYSCFG->EXTICR[3]  |= SYSCFG_EXTICR4_EXTI5_PF;   //linie 12-15

	RCC->APB2ENR &= ~RCC_APB2ENR_SYSCFGEN;

	NVIC_EnableIRQ(EXTI0_1_IRQn);          //włącz przerwania linie 0,1
  NVIC_SetPriority(EXTI0_1_IRQn,0);      //ustaw priorytet linii 0,1
  NVIC_EnableIRQ(EXTI2_3_IRQn);          //włącz przerwania linie 2,3
  NVIC_EnableIRQ(EXTI4_15_IRQn);         //włącz przerwania linie 4-15

}
//==========================================================================

