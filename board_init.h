/// *********************************************************
/// ******  Prototypy funkcji                     ***********
/// *********************************************************
//zasada: te funkcje które są związane z płytką
#include "stm32f0xx.h"  //definicje rejestrów - \Libraries\CMSIS

void GPIO_init(void);
void waitus(uint32_t );
void USART1_init(uint32_t);
void USART2_init(uint32_t);
void RTC_cfg(void);
void wr164(uint8_t);
void TIM3_init(void);
void TIM16_init(void);
void SD_LowLevel_Init(void);
int  SD_TotalSize(void);
void rtc_send(void);

void i2c2_Init(void);
void i2c_wr(uint8_t adr , uint8_t sub , uint16_t il);
void i2c_rd(uint8_t adr , uint8_t sub , uint16_t il);
void ds1307_rd(void);

void SPI2_init(uint8_t);

